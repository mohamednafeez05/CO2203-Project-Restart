$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$buildFolder = Join-Path $projectRoot 'build'
New-Item -ItemType Directory -Force $buildFolder | Out-Null
$testRunFolder = Join-Path ([System.IO.Path]::GetTempPath()) ('CO2203-tests-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory $testRunFolder | Out-Null
$projectSources = @(Get-ChildItem (Join-Path $projectRoot 'src/*.cpp') | Where-Object { $_.Name -ne 'main.cpp' } | ForEach-Object { $_.FullName })
$includeFolder = Join-Path $projectRoot 'include'
New-Item -ItemType Directory -Force (Join-Path $testRunFolder 'data/persistence_test') | Out-Null
Push-Location $projectRoot
try {
    & g++ -std=c++17 -Wall "-I$includeFolder" @projectSources (Join-Path $projectRoot 'src/main.cpp') -o (Join-Path $projectRoot 'mainApp.exe')
    if ($LASTEXITCODE -ne 0) { throw 'Application build failed.' }
    foreach ($testName in @('SafeSaveTest', 'PersistenceTest', 'StudentHistoryTest', 'IntegrationTest')) {
        $executable = Join-Path $buildFolder ($testName + '.exe')
        & g++ -std=c++17 -Wall "-I$includeFolder" (Join-Path $projectRoot "tests/$testName.cpp") @projectSources -o $executable
        if ($LASTEXITCODE -ne 0) { throw "Build failed: $testName" }
        Push-Location $testRunFolder
        try {
            & $executable
            if ($LASTEXITCODE -ne 0) { throw "Test failed: $testName" }
        } finally { Pop-Location }
    }
    Write-Host 'Application build and all four test suites passed.'
    Write-Host "Test data retained at: $testRunFolder"
} finally { Pop-Location }
