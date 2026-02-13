(ns aoc-2b.core)
(require 'clojure.string)

;; re-use part 1 solution, just replacing keypad
;;
;; FOR each instruction:
;;   IF can move:
;;     MOVE in keypad
;;   ELSE:
;;     STAY in position
;;   IF at end of line:
;;     STORE button at that position
;;
;; RETURN button presses after all instructions

(def test-input "ULL
RRDDD
LURDL
UUUUD
")
(def input-file (slurp "../aoc_2a/input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn get-button [position]
  (case position
    [2 -2] "1"
    [1 -1] "2"
    [2 -1] "3"
    [3 -1] "4"
    [0  0] "5"
    [1  0] "6"
    [2  0] "7"
    [3  0] "8"
    [4  0] "9"
    [1  1] "A"
    [2  1] "B"
    [3  1] "C"
    [2  2] "D"
    0))

(defn get-direction [char]
  (case char
    "U"  [0  -1]
    "R"  [1   0]
    "D"  [0   1]
    "L"  [-1  0]
    "\n" [0   0]))

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/replace #"\r" "")
      (clojure.string/replace #" " "")
      (clojure.string/split #"")))

(defn valid-move? [vec new-vec]
  (if (= (get-button new-vec) 0) vec new-vec))

(defn move-if-valid [vec char]
  (->> char
       get-direction
       (mapv + vec)
       (valid-move? vec)))

(defn get-positions [input]
  (rest
   (->> input
        (reductions move-if-valid [0 0]))))

(defn get-codes [input positions]
  (->> (map vector input positions)
       (filter (fn [[x _]] (= x "\n")))
       (map second)))

(defn get-bathroom-code [input]
  (->> input
       clean-and-split-input
       get-positions
       (get-codes (clean-and-split-input input))
       (map get-button)
       (clojure.string/join #"")))

(defn -main []
  (let [start (System/nanoTime)
        code (get-bathroom-code input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The bathroom code is:" code)))

;; (-main)
