# Java Delaunay Implementation

## About
All Java source is a derivative port of "Relatively Robust Divide and Conquer 2D Delaunay Construction Algorithm" by Wael El Oraiby. Porting work was done by David V. Makray.

## Usage
The Delaunay implementation is wraped in the Delaunay class in Delaunay.java. The GUI demonstration code is in JavaDelaunay.java and MainScreen.java. Compile all Java source to byte code and run JavaDelaunay.

Run JavaDelaunay from the Java source directory in a Bash terminal by typing:
```
javac Delaunay.java JavaDelaunay.java MainScreen.java
java JavaDelaunay
```

## Processing Usage
The Delaunay Java class can be used in Processing! Prepare a .JAR file from the Java source directory by running the following:
```
javac Delaunay.java
zip Delaunay.jar Delaunay*.class
```

In Processing, click on the _Sketch_ drop down menu, and click "Add File". Choose the .JAR file you created and the GUI will copy it into your sketch's code/ directory. Use the code from MainScreen.java in PaintComponent() as reference to begin your Delaunay sketching!


## Source Code Status
The Java Delaunay code is more-or-less a straight port from the C implementation and can be refactored after further studying of the algorithm. This port assumes that all passed point data is untamed and manages the input points by removing duplicates to avoid errors.
