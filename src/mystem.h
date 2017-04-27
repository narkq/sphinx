#pragma once

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef void MystemAnalysesHandle;
typedef void MystemLemmaHandle;
typedef void MystemFormsHandle;
typedef void MystemFormHandle;
typedef unsigned short int TSymbol;

MystemAnalysesHandle* MystemAnalyze(TSymbol* word, int len);
void MystemDeleteAnalyses(MystemAnalysesHandle* analyses);
int MystemAnalysesCount(MystemAnalysesHandle* analyses);

MystemLemmaHandle* MystemLemma(MystemAnalysesHandle* analyses, int i);

TSymbol* MystemLemmaText(MystemLemmaHandle* lemma);
int     MystemLemmaTextLen(MystemLemmaHandle* lemma);
TSymbol* MystemLemmaForm(MystemLemmaHandle* lemma);
int     MystemLemmaFormLen(MystemLemmaHandle* lemma);
int     MystemLemmaQuality(MystemLemmaHandle* lemma);
char*   MystemLemmaStemGram(MystemLemmaHandle* lemma);
char**  MystemLemmaFlexGram(MystemLemmaHandle* lemma);
int     MystemLemmaFlexGramNum(MystemLemmaHandle* lemma);
int     MystemLemmaFlexLen(MystemLemmaHandle* lemma);
int     MystemLemmaRuleId(MystemLemmaHandle* lemma);

MystemFormsHandle* MystemGenerate(MystemLemmaHandle* lemma);
void MystemDeleteForms(MystemFormsHandle* forms);
int MystemFormsCount(MystemFormsHandle* forms);

MystemFormHandle* MystemForm(MystemFormsHandle* forms, int i);

TSymbol* MystemFormText(MystemFormHandle* form);
int     MystemFormTextLen(MystemFormHandle* form);
char*   MystemFormStemGram(MystemFormHandle* form);
char**  MystemFormFlexGram(MystemFormHandle* form);
int     MystemFormFlexGramNum(MystemFormHandle* form);

typedef MystemAnalysesHandle* (*MystemAnalyze_fn) (TSymbol* word, int len);
typedef void (*MystemDeleteAnalyses_fn) (MystemAnalysesHandle* analyses);
typedef int (*MystemAnalysesCount_fn) (MystemAnalysesHandle* analyses);
typedef MystemLemmaHandle* (*MystemLemma_fn) (MystemAnalysesHandle* analyses, int i);
typedef TSymbol* (*MystemLemmaText_fn) (MystemLemmaHandle* lemma);
typedef int (*MystemLemmaTextLen_fn) (MystemLemmaHandle* lemma);
typedef TSymbol* (*MystemLemmaForm_fn) (MystemLemmaHandle* lemma);
typedef int      (*MystemLemmaFormLen_fn) (MystemLemmaHandle* lemma);
typedef int      (*MystemLemmaQuality_fn) (MystemLemmaHandle* lemma);
typedef char*    (*MystemLemmaStemGram_fn) (MystemLemmaHandle* lemma);
typedef char**   (*MystemLemmaFlexGram_fn) (MystemLemmaHandle* lemma);
typedef int      (*MystemLemmaFlexGramNum_fn) (MystemLemmaHandle* lemma);
typedef int      (*MystemLemmaFlexLen_fn) (MystemLemmaHandle* lemma);
typedef int      (*MystemLemmaRuleId_fn) (MystemLemmaHandle* lemma);
typedef MystemFormsHandle* (*MystemGenerate_fn) (MystemLemmaHandle* lemma);
typedef void (*MystemDeleteForms_fn) (MystemFormsHandle* forms);
typedef int (*MystemFormsCount_fn) (MystemFormsHandle* forms);
typedef MystemFormHandle* (*MystemForm_fn) (MystemFormsHandle* forms, int i);
typedef TSymbol* (*MystemFormText_fn) (MystemFormHandle* form);
typedef int      (*MystemFormTextLen_fn) (MystemFormHandle* form);
typedef char*    (*MystemFormStemGram_fn) (MystemFormHandle* form);
typedef char**   (*MystemFormFlexGram_fn) (MystemFormHandle* form);
typedef int      (*MystemFormFlexGramNum_fn) (MystemFormHandle* form);

#if defined(__cplusplus)
}
#endif
