shader: src/shader/*

src/shader/*:
	glslc -c $@ -o assets/shader/$(@F).spv
#alternativ:
#src/shader/*:
#	glslangValidator -V $@ -o assets/shader/$(@F).spv

#shader: src/shader/*
#	glslc -c $?