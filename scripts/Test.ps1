$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$buildFolder = Join-Path $projectRoot 'build'
$includeFolder = Join-Path $projectRoot 'include'
New-Item -ItemType Directory -Force $buildFolder | Out-Null
$testRunFolder = Join-Path ([System.IO.Path]::GetTempPath()) ('CO2203-tests-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Force (Join-Path $testRunFolder 'data/persistence_test') | Out-Null
$objects = @()
foreach ($source in Get-ChildItem (Join-Path $projectRoot 'src/*.cpp')) {
    if ($source.Name -eq 'main.cpp') { continue }
    $objectFile = Join-Path $buildFolder ($source.BaseName + '.o')
    & g++ -std=c++17 -Wall "-I$includeFolder" -c $source.FullName -o $objectFile
    if ($LASTEXITCODE -ne 0) { throw "Build failed: $($source.Name)" }
    $objects += $objectFile
}
& g++ -std=c++17 -Wall "-I$includeFolder" (Join-Path $projectRoot 'src/main.cpp') @objects -o (Join-Path $projectRoot 'mainApp.exe')
if ($LASTEXITCODE -ne 0) { throw 'Application build failed.' }
foreach ($testName in @('SafeSaveTest', 'PersistenceTest', 'StudentHistoryTest', 'IntegrationTest', 'CourseStorageTest', 'RegistrationConsoleTest', 'AttendanceImportTest', 'FinalSystemTest')) {
    $executable = Join-Path $buildFolder ($testName + '.exe')
    & g++ -std=c++17 -Wall "-I$includeFolder" (Join-Path $projectRoot "tests/$testName.cpp") @objects -o $executable
    if ($LASTEXITCODE -ne 0) { throw "Build failed: $testName" }
    Push-Location $testRunFolder
    try {
        & $executable
        if ($LASTEXITCODE -ne 0) { throw "Test failed: $testName" }
    } finally { Pop-Location }
}
Write-Host 'Application build and all eight test suites passed.'
Write-Host "Test data retained at: $testRunFolder"
