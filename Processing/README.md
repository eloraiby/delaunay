# Delaunay Processing Sketch Example

## About
This is a demonstration [Processing](http://www.processing.org) Sketch using the Java Delaunay implementation.

## Processing Usage
After downloading and installing Processing 3, you need to create a .jar file with the needed .class files. Create the .JAR file from the Java source directory by running the following in a Bash terminal:
```
javac Delaunay.java
zip Delaunay.jar Delaunay*.class
```

Take the DelaunayTest.pde file and place it into a "DelaunayTest" folder to match the name of the .pde file. In Processing, open the sketch from that directory, click on the _Sketch_ drop down menu, and click "Add File". Choose the .jar file you created and the GUI will copy it into your sketch's code/ directory. Click on the run button and Processing will run the program!
