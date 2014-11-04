AlphaCompiler
=============

Description
-------------
Compiler for the programming language Alpha, which is a subset of the Delta programming language.

A project I developed with 2 fellow-students (Klimatsidas Aleksis and Christakis Giannis) as a undergraduate student on Computer Science at the University of Crete.
It has some bugs on the Virtual Machine that runs the executables and they won't be fixed as the project is discontinued.

You can find more about The Delta programming language at : http://www.ics.forth.gr/hci/files/plang/Delta/Delta.html

Dependences
-----------

1) flex
2) bison

How To
======
 1) Compiler

	- Compile:
		a) With BackPatching
			$ make
			
		b) Without BackPatching
			$ make nopatch
			
	- Run :
		$ ./analyzer <inputFile>
	
2) AVM

	- Compile:
		$ make
		
	- Run: 
		$ alpha <Output from the Compiler/Run>
		

