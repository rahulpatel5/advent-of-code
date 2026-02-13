(ns aoc-2a.core)
(require 'clojure.string)

;; FOR each instruction:
;;   IF can't move to valid button:
;;     IGNORE move and move to next instruction
;;   ELSE:
;;     MOVE to button
;;   PRESS button at end of line
;;
;; RETURN buttons pressed for instructions

(def test-input "ULL
RRDDD
LURDL
UUUUD
")
(def input-file (slurp "input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      ;; need to get rid of a pesky carriage retrun
      (clojure.string/replace #"\r" "")
      (clojure.string/replace #" " "")
      (clojure.string/split #"")))

(defn get-button [[x y]]
  (case [x y]
    [-1 -1] 1
    [0  -1] 2
    [1  -1] 3
    [-1  0] 4
    [0   0] 5
    [1   0] 6
    [-1  1] 7
    [0   1] 8
    [1   1] 9
    0))

(defn move-instruction [char]
  (case char
    "U"  [0  -1]
    "R"  [1   0]
    "D"  [0   1]
    "L"  [-1  0]
    "\n" [0   0]))

(defn valid-move? [vec new-vec]
  (if (= (get-button new-vec) 0) vec new-vec))

(defn move-if-valid [vec char]
  (->> char
       move-instruction
       ;; try moving in the direction
       (mapv + vec)
       ;; if we're not at a known button, don't move
       (valid-move? vec)))

(defn get-position-at-each-step [input]
  (rest
   (->> input
        (reductions move-if-valid [0 0]))))

(defn get-code [input positions]
  (->> (map vector input positions)
       (filter (fn [[x _]] (= x "\n")))
       (map second)))

(defn get-bathroom-code [input]
  (->> input
       clean-and-split-input
       get-position-at-each-step
       (get-code (clean-and-split-input input))
       (map get-button)
       (clojure.string/join)))

(defn -main []
  (let [start (System/nanoTime)
        code (get-bathroom-code input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The bathroom code is:" code)))

;; (-main)
