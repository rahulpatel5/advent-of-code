(ns aoc-1b.core)
(require 'clojure.string)

;; repeat part 1 solution, but track position and find first
;; position that is duplicated
;;
;; FOR each instruction (direction):
;;   TRACK each location visited
;;
;; FOR each location visited:
;;   IDENTIFY first location visited twice
;; RETURN distance of first location visited twice from start

(def test-input "R8, R4, R4, R8")
(def input-file (slurp "../aoc_1a/input.txt"))
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
  (Integer. (clojure.string/join (rest instruction))))

(defn get-distances [input]
  (map get-number input))

(defn get-cardinals [input]
  (rest
   (reductions
    #(case %2
       \L (anti-clockwise %1)
       \R (clockwise %1))
    :north
    (map #(get % 0) input))))

(defn clean-and-split-input [input]
  (clojure.string/split
   (clojure.string/trim
    (clojure.string/replace input #" " ""))
   #","))

(defn collect-corners-visited [input]
  (reductions
   (fn [acc move-vec] (mapv + acc move-vec))
   [0 0]
   (map (fn [distance cardinal]
          (mapv #(* distance %) (get directions cardinal)))
        (get-distances input)
        (get-cardinals input))))

(defn get-range [num-a num-b]
  (if (< num-a num-b)
    (range (+ num-a 1) (+ num-b 1))
    (range (- num-a 1) (- num-b 1) -1)))

(defn add-horizontally [vec-a vec-b]
  (->> (get-range (second vec-a) (second vec-b))
       (map #(vector (first vec-a) %))))

(defn add-vertically [vec-a vec-b]
  (->> (get-range (first vec-a) (first vec-b))
       (map #(vector % (second vec-a)))))

(defn get-in-between-places [vec-a vec-b]
  (if (= (first vec-a) (first vec-b))
    (add-horizontally vec-a vec-b)
    (add-vertically vec-a vec-b)))

(defn get-full-place-list [places]
  (->> places
       (partition 2 1)
       (map (fn [[a b]] (get-in-between-places a b)))))

(defn find-duplicate [coll]
  (= (count coll) (count (set coll))))

(defn get-distance-to-first-place-visited-twice [input]
  (->> input
       clean-and-split-input
       ;; find each turn and then each block visited
       collect-corners-visited
       get-full-place-list
       (cons [[0 0]])
       ;; clean up collection
       (mapcat identity)
       (reductions conj [])
       ;; find where there's a duplicate and return the duplicate
       (drop-while find-duplicate)
       first
       last
       ;; finally, get the distance from the starting position
       ((fn [[a b]] (+ (abs a) (abs b))))))

(defn -main []
  (let [start (System/nanoTime)
        location (get-distance-to-first-place-visited-twice input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "Distance to first location visited twice:" location)))

;; (-main)
