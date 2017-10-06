#ifdef __cplusplus
extern "C" {
#endif


    // (OF)
char **rghStrs_newStrSep (
    char *str,  // (I)
    int *nStrs, // (I)
    char cSep); // (I)


void rghStrs_free (
    char **strs,  // (I)
    int nStrs);   // (I)


logical rghStrs_bStrElem (
    char **strs,  // (I)
    int nStrs,    // (I)
    char *str);   // (I)


void rghStrs_trace (
    char **strs,  // (I)
    int n,        // (I)
    char *id);    // (I)


#ifdef __cplusplus
}
#endif