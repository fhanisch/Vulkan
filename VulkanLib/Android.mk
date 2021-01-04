ANDROID_JAR=$(ANDROID_SDK)/platforms/android-29/android.jar
CC=$(NDK_HOME)/toolchains/llvm/prebuilt/windows-x86_64/bin/clang
CXX=$(NDK_HOME)/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++
AR=$(NDK_HOME)/toolchains/llvm/prebuilt/windows-x86_64/bin/aarch64-linux-android-ar
TARGET=--target=aarch64-linux-android29
SRCDIR=src
BUILDDIR=build/android
#SRC=$(SRCDIR)/AndroidMain.cpp $(SRCDIR)/VulkanSetup.cpp $(SRCDIR)/Models.cpp
SRC=$(SRCDIR)/android_native_app_glue.c $(SRCDIR)/AndroidMain.cpp $(SRCDIR)/VulkanSetup.cpp $(SRCDIR)/Models.cpp 
_OBJ=$(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.cpp.o, $(SRC))
OBJ=$(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.c.o, $(_OBJ))
LIB=$(BUILDDIR)/libapp_glue.a
LIBDIR=lib/arm64-v8a
SO=$(LIBDIR)/libmatrix.so
LIBNAME=libVulkanApp.so
CFLAGS=-Wall -fpic -DANDROID -DNOLOG
LDFLAGS=-landroid -llog -ldl

$(BUILDDIR)/%.cpp.o: $(SRCDIR)/%.cpp
	$(CXX) $(TARGET) -o $@ -c $< $(CFLAGS)

$(BUILDDIR)/%.c.o: $(SRCDIR)/%.c
	$(CC) $(TARGET) -o $@ -c $< $(CFLAGS)

$(BUILDDIR)/%.a: $(BUILDDIR)/android_native_app_glue.c.o
	$(AR) rcv $@ $<

$(LIBDIR)/%.so: $(BUILDDIR)/matrix.c.o
	$(CC) $(TARGET) -shared -o $@ $< -Wall

$(LIBDIR)/$(LIBNAME): $(OBJ) $(SO)
#Für dynamisches Linken der 'libc++' mit clang++ muss die 'libc++_shared.so' in das APK inkludiert werden!		
	$(CXX) $(TARGET) -shared -o $@ $(OBJ) $(LDFLAGS) -lmatrix -Wall -L./$(LIBDIR)
#$(LIBDIR)/$(LIBNAME): $(OBJ) $(LIB) $(SO)
#$(CXX) $(TARGET) -shared -o $@ $(OBJ) $(LDFLAGS) -lapp_glue -lmatrix -Wall -L./$(LIBDIR) -L./$(BUILDDIR) -u ANativeActivity_onCreate -static-libstdc++
#$(CXX) $(TARGET) -shared -o $@ $(OBJ) $(LDFLAGS) -lapp_glue -lmatrix -Wall -L./$(LIBDIR) -L./$(BUILDDIR) -u ANativeActivity_onCreate

output.apk: $(LIBDIR)/$(LIBNAME) AndroidManifest.xml
	aapt2 link -o output.apk -I $(ANDROID_JAR) --manifest AndroidManifest.xml
	7z a output.apk lib/ assets/

VulkanApp_unsigned-aligned.apk: output.apk
	del VulkanApp_unsigned-aligned.apk
	zipalign -v -p 4 output.apk VulkanApp_unsigned-aligned.apk

VulkanApp.apk: VulkanApp_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out VulkanApp.apk VulkanApp_unsigned-aligned.apk

VulkanApp: VulkanApp.apk