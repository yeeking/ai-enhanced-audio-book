## Command line programs to test rtneural and libtorch

This project provides a python script that will read in a saved model from the 
other training project (037a_train_lstm) and export it to torchscript and rtneural json formats:

* exporter.py

The project also provides two  command line programs, defined in :

* src/main_rtneural_basic.cpp : minimal program to load an rtneural model weights from JSON and  pass a signal through the resulting model 
* src/main_torchscript_vs_rtneural.cpp : program that creates one model from RTneural and one from torchscript, then shows that they produce the same output for the same input

