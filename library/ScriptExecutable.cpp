#include "ScriptExecutable.h"
#include <Python.h>
#include <iostream>


static PyObject* execute(PyObject *self, PyObject *args)
{
	std::cout << "execute called!" << std::endl;
	return Py_None;
}

static PyMethodDef module_methods[] = {
	{"execute", ::execute, METH_VARARGS, "Execute another script/calculation method by name."},
	{NULL, NULL, 0, NULL}
};


ScriptExecutable::ScriptExecutable(const std::wstring& script_name)
{
	script_name_.resize(script_name.size()+1); // +1 for a null terminator
	size_t num_conv = 0;
	if(wcstombs_s(&num_conv, &script_name_[0], script_name_.size(), &script_name[0], script_name.size()) == 0)
		script_name_.resize(num_conv-1);
	else
		script_name_ = "";
}

ScriptExecutable::~ScriptExecutable(void)
{
}

int ScriptExecutable::Execute(int param)
{
	if(script_name_.empty())
		return -1;

	FILE* file = fopen((script_name_+".py").c_str(), "rt");
	if(!file)
		return -1;
	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	std::string str;
	str.resize(file_size);
	fread(&str[0], 1, file_size, file);
	fclose(file);

	if(!Py_IsInitialized())
		Py_Initialize();

	PyObject* module = Py_InitModule("frames", module_methods);

	int ret = PyRun_SimpleString(str.c_str());
	if(ret == -1)
		PyErr_Print();

	return ret;
}
