
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>


// struct Matrix {
//     long x;
//     long y;
//     long* a;
//     // long* refc;
// };


// Matrix MatrixNew(long x, long y){
//     Matrix m;
//     m.x = x;
//     m.y = y;
//     m.a = (long*)malloc(x * y);
//     return m;
// }

// long MatrixLen(Matrix m){
//     return m.x * m.y;
// }

// Matrix MatrixCopy(Matrix m){
//     Matrix c;
//     c.x = m.x;
//     c.y = m.y;
//     c.a = memcpy(c.a, m.a, MatrixLen(m));
//     return c;
// }

// void MatrixDestroy(Matrix m){
//     free(m.a);
// }


// long MatrixAt(Matrix* m, long x, long y){
//     return m[y * m->x + x];
// }

// long MatrixAtI(Matrix* m, long i){
//     return m[i];
// }

// void MatrixSet(Matrix* m, long x, long y, long v){
//     m[y * m->x + x] = v;
// }


// void MatrixSetI(Matrix* m, long i, long v){
//     m[i] = v;
// }


// PyObject* to_pylist(Matrix* m){
//     PyObject* l = PyList_New(sz);

//     for (long i = 0; i < MatrixLen(m); i++){
//         PyList_SetItem(l, i, PyLong_FromLong(MatrixAtI(i)));
//     }

//     return l;
// }

// Matrix* from_pyobject(PyObject* p, long size_x, long sisze_y){
//     long sz = PyList_Size(p);

//     for (long i = 0; i < sz; i++){
//         MatrixSetI(p, i, PyList_GetItem(p, i));
//     }


// }


static PyObject*
personal_system(PyObject* self, PyObject* args){
    const char* command;
    int status;

    // If argument is not a tuple fail
    if(!PyArg_ParseTuple(args, "s", &command)){
        return NULL;
    }

    // Do what we want to do
    status = system(command);


    // Return value for python
    return PyLong_FromLong(status);
}

static PyObject*
personal_full_list(PyObject* self, PyObject* args){

    PyObject* maybe_sz = PySequence_GetItem(args,0);

    if (!PyLong_Check(maybe_sz)) {
        printf("Fatal: Args is not a a number\n");
        return NULL;
    }

    long sz = PyLong_AsLong(maybe_sz);

    PyObject* l = PyList_New(sz);

    for (long i = 0; i < sz; i++){
        PyList_SetItem(l, i, PyLong_FromLong(0));
    }

    return l;
}

static double 
average(PyObject* sequence){
    long sz = PySequence_Size(sequence); 

    double sum = 0;
    
    for (long i = 0; i < sz; i++){
        sum += PyFloat_AsDouble(PySequence_GetItem(sequence, i));
    }

    return sum / sz;
}

static double 
std_dev_sqr(PyObject* sequence){

    long sz = PySequence_Size(sequence); 

    double dev = 0;
    double avg = average(sequence);
    for (long i = 0; i < sz; i++){
        dev += pow(PyFloat_AsDouble(PySequence_GetItem(sequence, i)) - avg, 2);
    }

    return dev / (sz-1);
}

static double 
std_dev(PyObject* sequence){
    return sqrt(std_dev_sqr(sequence));
}

static PyObject*
personal_fit_line(PyObject* self, PyObject* args){

    long args_sz = PySequence_Size(args); 

    if (args_sz != 2){
        return NULL;
    }
    
    PyObject* seq_x = PySequence_GetItem(args,0);
    PyObject* seq_y = PySequence_GetItem(args,1);
    
    if (!PySequence_Check(seq_x)){
        return NULL; 
    }

    if (!PySequence_Check(seq_y)){
        return NULL; 
    }

    long sz = PySequence_Size(seq_x);
    if (sz != PySequence_Size(seq_y)){
        return NULL;
    }


    double s = sz;
    double sx = 0;
    double sy = 0;
    double sxx = 0;
    double syy = 0;
    double sxy = 0;

    double dev_sqr = std_dev_sqr(seq_y);
    /* 
        Math sourced from:
        https://pl.wikipedia.org/wiki/Metoda_najmniejszych_kwadrat%C3%B3w
    */

    for (long i = 0; i < sz; i++){
        /* 
            Not checking for the type here it will just crash,
            but should be faster this way
        */
        double x = PyFloat_AsDouble(PySequence_GetItem(seq_x, i));
        double y = PyFloat_AsDouble(PySequence_GetItem(seq_y, i));

        sx = x;
        sy = y;
        sxx = pow(x, 2);
        syy = pow(y, 2);
        sxy = x * y;
    }

    double delta = s * sxx - pow(sx, 2);

    double a = (s * sxy - sx * sy) / delta; 
    double b = (sxx * sy - sx * sxy) / delta; //TODO: I dont think 'b' is calculated corectly

    double r = (s*sxy - sx * sy) / sqrt( (s * sxx - pow(sx,2) * (s * syy - pow(sy,2))) );

    PyObject* return_tuple = PyTuple_New(3);
    PyTuple_SET_ITEM(return_tuple, 0, PyFloat_FromDouble(a));
    PyTuple_SET_ITEM(return_tuple, 1, PyFloat_FromDouble(b));
    PyTuple_SET_ITEM(return_tuple, 2, PyFloat_FromDouble(r));


    return return_tuple;    
}

static short g_counter = 0;

static PyObject*
personal_increment(PyObject* self, PyObject* args){
    g_counter++;
    return PyLong_FromLong(g_counter);
}

// Define functions in module
static PyMethodDef PersonalMethods[] = {
    {"system", personal_system, METH_VARARGS, "Execute command in shell but mine"},
    {"fit_line", personal_fit_line, METH_VARARGS, "Fit the line between two arrays"},
    {"full_list", personal_full_list, METH_VARARGS, "Create full list of given size"},
    {"increment", personal_increment, METH_VARARGS, "Increment global counter"},
    {NULL, NULL, 0, NULL} // Sentinel value
};

// Informations about module
static struct PyModuleDef personalmodule = {
    PyModuleDef_HEAD_INIT,
    "personal",
    "My personal python functions that neded to be a bit more optimized",
    -1,
    PersonalMethods
};

// Initialize the module
// WARN: This function needs to follow convention: PyInit_nameofthemodule
PyMODINIT_FUNC
PyInit_personal(void){ 
    return PyModule_Create(&personalmodule);
}


int
main(int argc, char *argv[])
{
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    if (PyImport_AppendInittab("personal", PyInit_personal) == -1) {
        fprintf(stderr, "Error: could not extend in-built modules table\n");
        exit(1);
    }

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required.
       If this step fails, it will be a fatal error. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyObject *pmodule = PyImport_ImportModule("personal");
    if (!pmodule) {
        PyErr_Print();
        fprintf(stderr, "Error: could not import module 'spam'\n");
    }


    PyMem_RawFree(program);
    return 0;
}