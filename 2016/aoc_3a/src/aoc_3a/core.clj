(ns aoc-3a.core)
(require 'clojure.string)

;; FOR each line:
;;   IF valid triangle:
;;     INCREMENT valid count
;;
;; RETURN count of valid triangles
;;
;; when checking for two sides being larger than the third
;; need to make sure not to discount isosceles or equilateral
;; i.e. if removing the longest side before calculating with other sides

(def test-input "5 10 25")
(def input-file (slurp "input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn get-numbers [str-seq]
  (as-> str-seq s
    (clojure.string/trim s)
    (clojure.string/split s #"\s+")
    (map parse-long s)))

(defn clean-and-split-input [input]
  (as-> input s
    (clojure.string/replace s #"\r" "")
    (clojure.string/split-lines s)
    (map get-numbers s)))

(defn get-largest-number [input]
  (reduce max input))

(defn get-index-of-first-largest-number [largest-number vec]
  (.indexOf vec largest-number))

(defn remove-largest-side [vec largest-index]
  (keep-indexed (fn [i v] (when (not= i largest-index) v)) vec))

(defn get-sum-of-smaller-triangle-sides [vec largest-index]
  (reduce + (remove-largest-side vec largest-index)))

(defn get-count-of-valid-triangles [sum-smaller-sides largest-side]
  (if (> sum-smaller-sides largest-side) 1 0))

(defn count-valid-triangles [input]
  (let [clean-input (clean-and-split-input input)
        largest-numbers (map get-largest-number clean-input)
        largest-number-indexes (map get-index-of-first-largest-number largest-numbers clean-input)
        sum-of-smaller-sides (map get-sum-of-smaller-triangle-sides clean-input largest-number-indexes)]
    (reduce + (map get-count-of-valid-triangles sum-of-smaller-sides largest-numbers))))

(defn -main []
  (let [start (System/nanoTime)
        triangles (count-valid-triangles input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "Number of valid triangles:" triangles)))

;; (-main)
