shader: src/shader/*

src/shader/*:
	glslc -c $@ -o res/shader/$(@F).spv
#alternativ:
#src/shader/*:
#	glslangValidator -V $@ -o res/shader/$(@F).spv

#shader: src/shader/*
#	glslc -c $?