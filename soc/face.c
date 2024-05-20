#include "Python.h"

void python_init(){
    Py_Initialize();
    PyObject *sys = PyImport_ImportModule("sys");//sys = import sys
    PyObject *path = PyObject_GetAttrString(sys, "path");//path = sys.path
    PyList_Append(path, PyUnicode_FromString("/home/orangepi/yuanxueshe/smart_home/face_recg"));//将当前路径添加到pyth    on路径
}
void python_final(){
    Py_Finalize();

}
double face_recg(char *model, char *function){
    double ret;
    PyObject *pModule = PyImport_ImportModule(model);//导入nopara模块，返回空为失败
    if(!pModule){
        PyErr_Print();
        printf("Error: failed to load module %s\n", model);
        goto FAILED_MODULE;
    }
    PyObject *pFunc = PyObject_GetAttrString(pModule, function);
    if (!pFunc)
    {
        PyErr_Print();
        printf("Error: failed to load alibabacloud_face\n");
        goto FAILED_FUNC;
    }
    PyObject *pValue = PyObject_CallObject(pFunc,NULL);//检查pfunc是否调用成功，把pargs传给pfunc返回ture fals    e
    if (!pValue)
    {
        PyErr_Print();
        printf("Error: function call failed\n");
        goto FAILED_VALUE;
    }
    if (!PyArg_Parse(pValue, "d", &ret))
    {
        PyErr_Print();
        printf("Error: parse failed");
        goto FAILED_RESULT;
    }

FAILED_RESULT:
    Py_DECREF(pValue);
FAILED_VALUE:
    Py_DECREF(pFunc);
FAILED_FUNC:
    Py_DECREF(pModule);//释放引入的模块，后引入的先释放
FAILED_MODULE:
    return ret;
}


