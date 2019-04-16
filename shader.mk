Shader:
	glslangValidator -V Basic\src\shader\default.vert -o x64\Debug\vs_default.spv
	glslangValidator -V Basic\src\shader\text.vert -o x64\Debug\vs_text.spv
	glslangValidator -V Basic\src\shader\perlinCircle.vert -o x64\Debug\vs_perlinCircle.spv
	glslangValidator -V Basic\src\shader\welle.vert -o x64\Debug\vs_welle.spv
	glslangValidator -V Basic\src\shader\perlin1d.vert -o x64\Debug\vs_perlin1d.spv
	glslangValidator -V Basic\src\shader\graph.vert -o x64\Debug\vs_graph.spv
	glslangValidator -V Basic\src\shader\test.frag -o x64\Debug\fs_test.spv
	glslangValidator -V Basic\src\shader\default.frag -o x64\Debug\fs_default.spv
	glslangValidator -V Basic\src\shader\text.frag -o x64\Debug\fs_text.spv
	glslangValidator -V Basic\src\shader\powermeter.frag -o x64\Debug\fs_powermeter.spv
	glslangValidator -V Basic\src\shader\perlin2d.frag -o x64\Debug\fs_perlin2d.spv
	glslangValidator -V Basic\src\shader\circleFilled.frag -o x64\Debug\fs_circleFilled.spv
	glslangValidator -V Basic\src\shader\graph.frag -o x64\Debug\fs_graph.spv