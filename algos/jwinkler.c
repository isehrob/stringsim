// Jaro Winkler distance algo source is copied from https://github.com/miguelvps/c/blob/master/jarowinkler.c

// Python C extension for use in personal projects


#include <Python.h>
#include <string.h>

#define SCALING_FACTOR 0.1

static int max(int x, int y) {
    return x > y ? x : y;
}

static int min(int x, int y) {
    return x < y ? x : y;
}

double jaro_winkler_distance(const char *s, const char *a) {
    int i, j, l;
    int m = 0, t = 0;
    int sl = strlen(s);
    int al = strlen(a);
    int sflags[sl], aflags[al];
    int range = max(0, max(sl, al) / 2 - 1);
    double dw;

    if (!sl || !al)
        return 0.0;

    for (i = 0; i < al; i++)
        aflags[i] = 0;

    for (i = 0; i < sl; i++)
        sflags[i] = 0;

    /* calculate matching characters */
    for (i = 0; i < al; i++) {
        for (j = max(i - range, 0), l = min(i + range + 1, sl); j < l; j++) {
            if (a[i] == s[j] && !sflags[j]) {
                sflags[j] = 1;
                aflags[i] = 1;
                m++;
                break;
            }
        }
    }

    if (!m)
        return 0.0;

    /* calculate character transpositions */
    l = 0;
    for (i = 0; i < al; i++) {
        if (aflags[i] == 1) {
            for (j = l; j < sl; j++) {
                if (sflags[j] == 1) {
                    l = j + 1;
                    break;
                }
            }
            if (a[i] != s[j])
                t++;
        }
    }
    t /= 2;

    /* Jaro distance */
    dw = (((double)m / sl) + ((double)m / al) + ((double)(m - t) / m)) / 3.0;

    /* calculate common string prefix up to 4 chars */
    l = 0;
    for (i = 0; i < min(min(sl, al), 4); i++)
        if (s[i] == a[i])
            l++;

    /* Jaro-Winkler distance */
    dw = dw + (l * SCALING_FACTOR * (1 - dw));

    return dw;
}

static PyObject* jwinkler_similarity(PyObject* self, PyObject* args)
{
    const char *s1;
    const char *s2;
    double res;

    if(!PyArg_ParseTuple(args, "ss", &s1, &s2))
        return NULL;

    res = jaro_winkler_distance(s1, s2);
    return Py_BuildValue("f", res);
}

static PyMethodDef jw_methods[] = {
    { "jwinkler_similarity", jwinkler_similarity, METH_VARARGS, "Calculates Jaro Winkler similarity" },
    { NULL, NULL, 0, NULL }
};

static struct PyModuleDef jwinkler_c = {
    PyModuleDef_HEAD_INIT,
    "jwinkler_c",
    "Jaro Winkler C extension for python",
    -1,
    jw_methods
};

PyMODINIT_FUNC PyInit_jwinkler_c(void)
{
    return PyModule_Create(&jwinkler_c);
}
