Project Plan: Enorase
Date: 5/12/15
Ryan Jenkins & Jonathan Olson

We cannot guarantee code compatability or optimal rendering on all devices.  For our screenshots, development we used Processing 2.2.1, and HTC M8 HarmenKarden Edition phone running Adnroid 5.0.1. 

To Compile and Run:
Open Enorase/SkullCracker project with Processing 2.2.1 in android mode.  In the Processing window select Android->Sketch Permissions and Android Permissions Selector will pop up.  Make sure Internet permissions is enabled(the internet permissions box is checked).  Before running the program, we suggest you enable WIFI connection on your Android device as it will need to connect with our server to download client information.  Once you have a satisfactory connection to the internet on your device, click the play button (Run on Device) and allow Processing to begin running the program.  When the text in the debugging window displays: Running on Device... the screen on your android device will go black as it syncronizes with the server.  This may take a minute or two depending on your internet connection.  When the screen displays the View Patient and Settings buttons, you are ready to explore!  

To Resync client files with server:
Click Settings->Update.  Make sure your android device has a good internet connection before making this selection.  It should take a few minutes to retrieve all patient data.

To compile the mesh construction portion of the project you'll need to use CMake and have ITK installed. Compile instructions for the C++ part of the project can be found in README.md at the root level of the repository and requires ITK. MCMain in the build target for mesh construction, so if that's all you want to use you can run `make MCMain` to only build that target and its dependencies. Run the MCMain executable that produces with no arguments for usage instructions. JSON that can be passed to the device will be written to stdout in a successful run and a .obj files for viewing will be written to the `output_file` if provided or `a.obj` in the working directory if not.
