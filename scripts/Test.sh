#!/usr/bin/env bash
set -euo pipefail
project_root="$(cd "$(dirname "$0")/.." && pwd)"
test_folder="$(mktemp -d)"
mkdir -p "$test_folder/data/persistence_test" "$project_root/build"
objects=()
for source in "$project_root"/src/*.cpp; do
    [[ "$source" == */main.cpp ]] && continue
    object="$test_folder/$(basename "${source%.cpp}").o"
    g++ -std=c++17 -Wall -I"$project_root/include" -c "$source" -o "$object"
    objects+=("$object")
done
g++ -std=c++17 -Wall -I"$project_root/include" "$project_root/src/main.cpp" "${objects[@]}" -o "$project_root/mainApp.exe"
for name in SafeSaveTest PersistenceTest StudentHistoryTest IntegrationTest CourseStorageTest RegistrationConsoleTest AttendanceImportTest FinalSystemTest; do
    g++ -std=c++17 -Wall -I"$project_root/include" "$project_root/tests/$name.cpp" "${objects[@]}" -o "$project_root/build/$name"
    (cd "$test_folder" && "$project_root/build/$name")
done
printf 'Application build and all eight test suites passed.\n'
printf 'Test data retained at: %s\n' "$test_folder"
