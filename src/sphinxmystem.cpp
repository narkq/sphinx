#include "sphinx.h"
#include "sphinxint.h"
#include "sphinxutils.h"
#include "mystem.h"
#ifdef HAVE_DLOPEN
#include <dlfcn.h>
#endif // HAVE_DLOPEN

#if !USE_WINDOWS
#ifndef HAVE_DLERROR
#define dlerror() ""
#endif // HAVE_DLERROR
#endif // !USE_WINDOWS

static void* g_hMystem = 0;
static MystemAnalyze_fn			g_fnMystemAnalyze = 0;
static MystemDeleteAnalyses_fn	g_fnMystemDeleteAnalyses = 0;
static MystemAnalysesCount_fn	g_fnMystemAnalysesCount = 0;
static MystemLemma_fn			g_fnMystemLemma = 0;
static MystemLemmaText_fn		g_fnMystemLemmaText = 0;
static MystemLemmaTextLen_fn	g_fnMystemLemmaTextLen = 0;

inline TSymbol* UTF8ToSymbols ( const BYTE* pWord, int len )
{
	TSymbol* pSymbols;
	int iPos = 0, iCode;
	const BYTE * p = pWord;

	pSymbols = (TSymbol*) malloc ( sizeof(TSymbol) * len );

	while ( ( iCode = sphUTF8Decode(p) )!=0 && iPos<len )
	{
		pSymbols[iPos] = iCode;
		iPos++;
	}

	return pSymbols;
}

inline void symbolsToUTF8 ( BYTE* pOut, const TSymbol* pWord, int len )
{
	int iPos;
	BYTE* p = pOut;

	for (iPos = 0; iPos < len; iPos++) {
		p += sphUTF8Encode ( p, pWord[iPos] );
	}

	*p = '\0';
}

void* loadMystemFunc ( const char* name, CSphString & sError )
{
	dlerror ();
	void* dlSymbol = dlsym ( g_hMystem, name );
	char* dlError = dlerror ();
	if ( dlError != NULL )
	{
		sError.SetSprintf ( "failed to load symbol %s: %s", name, dlError );
		return 0;
	}
	return dlSymbol;
}

bool mystemShutdown ()
{
	if ( g_hMystem )
		dlclose( g_hMystem );

	g_hMystem = 0;

	g_fnMystemAnalyze = 0;
	g_fnMystemAnalysesCount = 0;
	g_fnMystemLemma = 0;
	g_fnMystemLemmaText = 0;
	g_fnMystemLemmaTextLen = 0;
	g_fnMystemDeleteAnalyses = 0;

	return false;
}

bool sphMystemInit ( CSphString & sError )
{
#if !HAVE_DLOPEN
	sError = "no dlopen(), no mystem";
	return false;
#else
	if ( g_hMystem )
		return true;

	g_hMystem = dlopen("libmystem_c_binding.so", RTLD_NOW | RTLD_DEEPBIND);
	if ( !g_hMystem ) {
		sError.SetSprintf ( "failed to load libmystem: %s", dlerror() );
		return false;
	}

	g_fnMystemAnalyze = (MystemAnalyze_fn) loadMystemFunc ( "MystemAnalyze", sError );
	if ( !g_fnMystemAnalyze )
		return mystemShutdown ();

	g_fnMystemDeleteAnalyses = (MystemDeleteAnalyses_fn) loadMystemFunc ( "MystemDeleteAnalyses", sError );
	if ( !g_fnMystemDeleteAnalyses )
		return mystemShutdown ();

	g_fnMystemAnalysesCount = (MystemAnalysesCount_fn) loadMystemFunc ( "MystemAnalysesCount", sError );
	if ( !g_fnMystemAnalysesCount )
		return mystemShutdown ();

	g_fnMystemLemma = (MystemLemma_fn) loadMystemFunc ( "MystemLemma", sError );
	if ( !g_fnMystemLemma )
		return mystemShutdown ();

	g_fnMystemLemmaText = (MystemLemmaText_fn) loadMystemFunc ( "MystemLemmaText", sError );
	if ( !g_fnMystemLemmaText )
		return mystemShutdown ();

	g_fnMystemLemmaTextLen = (MystemLemmaTextLen_fn) loadMystemFunc ( "MystemLemmaTextLen", sError );
	if ( !g_fnMystemLemmaTextLen )
		return mystemShutdown ();

	return true;
#endif // HAVE_DLOPEN
}

inline void sphMystemAssert ()
{
	assert ( g_fnMystemAnalyze );
	assert ( g_fnMystemAnalysesCount );
	assert ( g_fnMystemLemma );
	assert ( g_fnMystemLemmaText );
	assert ( g_fnMystemLemmaTextLen );
	assert ( g_fnMystemDeleteAnalyses );
}

void sphMystemLemmatize ( BYTE * pWord )
{
	sphMystemAssert ();

	int iWordLen = sphUTF8Len ( (char*)pWord, SPH_MAX_WORD_LEN );
	TSymbol* pUnicodeWord = UTF8ToSymbols ( pWord, iWordLen );

	MystemAnalysesHandle* pAnalyses = g_fnMystemAnalyze ( pUnicodeWord, iWordLen );;
	int iAnalysesCount = g_fnMystemAnalysesCount ( pAnalyses );

	if ( iAnalysesCount > 0 )
	{
		MystemLemmaHandle* pLemma = g_fnMystemLemma ( pAnalyses, 0 );
		TSymbol* pLemmaText = g_fnMystemLemmaText ( pLemma );
		int iLemmaTextLen = g_fnMystemLemmaTextLen ( pLemma );

		symbolsToUTF8 ( pWord, pLemmaText, iLemmaTextLen );
	}

	g_fnMystemDeleteAnalyses ( pAnalyses );
    free ( pUnicodeWord );
}

void sphMystemLemmatize ( CSphVector<CSphString> & dLemmas, const BYTE * pWord )
{
	sphMystemAssert ();

	int iWordLen = sphUTF8Len ( (char*)pWord, SPH_MAX_WORD_LEN );
	TSymbol* pUnicodeWord = UTF8ToSymbols ( pWord, iWordLen );

	MystemAnalysesHandle* pAnalyses = g_fnMystemAnalyze ( pUnicodeWord, iWordLen );;
	int iAnalysesCount = g_fnMystemAnalysesCount ( pAnalyses );

	for ( int i=0; i<iAnalysesCount; i++ )
	{
		MystemLemmaHandle* pLemma = g_fnMystemLemma ( pAnalyses, i );
		TSymbol* pLemmaText = g_fnMystemLemmaText ( pLemma );
		int iLemmaTextLen = g_fnMystemLemmaTextLen ( pLemma );

		BYTE sRes [ 3*SPH_MAX_WORD_LEN+4 ];

		symbolsToUTF8 ( sRes, pLemmaText, iLemmaTextLen );
		dLemmas.Add ( (const char*)sRes );
	}

	g_fnMystemDeleteAnalyses ( pAnalyses );
    free ( pUnicodeWord );
}

void sphMystemShutdown ()
{
	mystemShutdown ();
}
