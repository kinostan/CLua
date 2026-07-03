$BaseDir = $PSScriptRoot;

g++ -std=c++26 "$BaseDir/bundler.cpp" -I"C:/dev/C_C++/StdToolset/" -I"src" -I"tests/lexer" -D_DEBUG -O0 -o "$BaseDir/test.exe" -g

if ($LASTEXITCODE -ne 0) {
    Write-Error "Compilation failed (exit code $LASTEXITCODE)"
    exit $LASTEXITCODE
}
