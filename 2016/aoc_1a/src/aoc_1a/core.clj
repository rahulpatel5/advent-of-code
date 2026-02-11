(ns aoc-1a.core)
(require 'clojure.string)

;; FOR each direction:
;;   TRACK current coordinate, relative to initial position [0 0]
;;
;; RETURN sum of absolute values in final coordinates

(def test-input "R5, L5, R5, R3")
(def input-file (slurp "input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(def directions {:north  [0 -1],
                 :east   [1  0],
                 :south  [0  1],
                 :west  [-1  0]})

(defn clockwise [direction]
  (case direction
    :north :east
    :east  :south
    :south :west
    :west  :north))

(defn anti-clockwise [direction]
  (case direction
    :north :west
    :east  :north
    :south :east
    :west  :south))

(defn get-number [instruction]
  (Integer. (clojure.string/replace instruction #"[LR]" "")))

(defn get-distances [input]
  ;; Get collection of distances (the numbers) from the input.
  (map
   #(get-number %)
   input))

(defn get-cardinals [input]
  ;; Get collection of compass directions from the input.
  (rest
   (reductions
    #(case %2
       \L (anti-clockwise %1)
       \R (clockwise %1))
    :north
    (map #(get % 0) input))))

(defn follow-instructions [input]
  ;; Find how far we move from initial position.
  (reduce
   (fn [acc move-vec] (mapv + acc move-vec))
   [0 0]
   (map (fn [distance cardinal]
          (mapv #(* distance %) (get directions cardinal)))
        (get-distances input)
        (get-cardinals input))))

(defn clean-and-split-input [input]
  (clojure.string/split
   ;; remove whitespace in puzzle input
   (clojure.string/trim
    (clojure.string/replace input #" " ""))
   #","))

(defn get-blocks-distance [input]
  (let [final-coordinates (follow-instructions (clean-and-split-input input))]
    (reduce + (map abs final-coordinates))))

(defn -main []
  (let [start (System/nanoTime)
        blocks (get-blocks-distance input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "Distance away, in blocks:" blocks)))

;; (-main)
