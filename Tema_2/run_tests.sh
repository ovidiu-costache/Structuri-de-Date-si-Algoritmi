#!/usr/bin/env bash
# Script pentru rularea testelor pentru tema2 cu punctaj și verificare Valgrind
# Ignoră diferențele de spații și linii goale
# Utilizare:
#   ./run_tests.sh            -> rulează toate task-urile 1-5
#   ./run_tests.sh 5          -> rulează doar task-ul 5

# Culoare pentru output
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Punctaj total pe cerințe (index 1..5)
declare -A TASK_POINTS=( [1]=30 [2]=20 [3]=20 [4]=25 [5]=20 )

# Verificăm că executabilul există
if [[ ! -x "tema2" ]]; then
  echo "Executabilul 'tema2' nu a fost găsit sau nu e executabil."
  exit 1
fi

# Structura de directoare
TASKS_DIR="tasks"
ALL_TASKS=(1 2 3 4 5)

# Dacă s-a trecut un argument, îl folosim ca listă de task-uri
if [[ -n "$1" ]]; then
  TASKS=($1)
else
  TASKS=(${ALL_TASKS[@]})
fi

# Variabile pentru scor global
global_tests=0
global_score=0

echo "Încep rularea testelor cu punctaj și Valgrind..."
for task in "${TASKS[@]}"; do
  echo "======================================"
  echo "Cerința $task (punctaj total: ${TASK_POINTS[$task]})"
  echo "======================================"

  TEST_DIR="$TASKS_DIR/task${task}/tests"
  if [[ ! -d "$TEST_DIR" ]]; then
    echo "Directorul $TEST_DIR nu există, sărim cerința $task."
    continue
  fi

  # Număr teste
  test_dirs=($TEST_DIR/*)
  task_total_tests=${#test_dirs[@]}
  # Puncte per test
  points_per_test=$(echo "scale=4; ${TASK_POINTS[$task]} / $task_total_tests" | bc)

  # Contoare per task
  task_tests=0
  task_score=0

  for test_dir in "${test_dirs[@]}"; do
    if [[ -d "$test_dir" ]]; then
      ((global_tests++))
      ((task_tests++))
      base=$(basename "$test_dir")
      in_file="$test_dir/$base.in"
      ref_file="$test_dir/$base.ref"
      out_file="$test_dir/$base.out"
      vg_log="$test_dir/vg.log"

      # Rulare normală și generare output
      ./tema2 -c${task} "$in_file" "$out_file"
      rc=$?
      if [[ $rc -ne 0 ]]; then
        echo -e "[${RED}ERROR${NC}] $base (exit: $rc)"
        continue
      fi

      # Comparare rezultate (ignoră spații și linii goale)
      if diff -u -w -B "$ref_file" "$out_file" > /dev/null; then
        # Verificare cu Valgrind: leaks/tracking errors
        valgrind --leak-check=full --error-exitcode=1 --log-file="$vg_log" \
          --quiet ./tema2 -c${task} "$in_file" "/dev/null"
        vg_rc=$?
        if [[ $vg_rc -eq 0 ]]; then
          # Fără erori de memorie
          award=$(printf "%.4f" "$points_per_test")
          echo -e "[${GREEN}OK${NC}] $base +${award} (Valgrind clean)"
        else
          # Erori de memorie, 90% din punctaj
          partial=$(echo "scale=4; $points_per_test * 0.9" | bc)
          echo -e "[${YELLOW}VALGRIND${NC}] $base +${partial} (90% din ${points_per_test})"
          award=$partial
        fi
        # Adăugăm punctaj
        task_score=$(echo "$task_score + $award" | bc)
        global_score=$(echo "$global_score + $award" | bc)
      else
        echo -e "[${RED}FAIL${NC}] $base"
      fi
    fi
  done

  # Rezumat per task
  echo "--- Cerința $task: $(printf "%.2f" "$task_score") puncte din maxim ${TASK_POINTS[$task]} (teste: $task_tests/$task_total_tests) ---"
  echo ""
done

# Rezumat global
echo "======================================"
echo "Scor total: $(printf "%.2f" "$global_score") puncte din 115."
echo "(Teste rulate: $global_tests)"
echo "======================================"