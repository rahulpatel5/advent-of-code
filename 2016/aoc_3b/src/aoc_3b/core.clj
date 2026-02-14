(ns aoc-3b.core)
(require 'clojure.string)

;; re-use part 1 solution, but change how triangles are read
;;
;; FOR each triangle:
;;   IF sum of smallest sides > largest side:
;;     INCREMENT count of valid triangles
;;
;; RETURN count of valid triangles

(def test-input "101 301 501
102 302 502
103 303 503
201 401 601
202 402 602
203 403 603
")
(def input-file (slurp "../aoc_3a/input.txt"))
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

(defn get-column-input [input]
  (->> (concat (map first input)
               (map second input)
               (map #(nth % 2) input))
       ;; we've got all elements in the right order
       ;; now split into 3s to get sides of triangles
       (partition 3)))

(defn get-largest-number [v]
  (reduce max v))

(defn get-index-of-largest-number [v largest-number]
  (.indexOf v largest-number))

(defn drop-longest-side [vect largest-index]
  (keep-indexed (fn [i v] (when (not= i largest-index) v)) vect))

(defn get-sum-of-smallest-triangle-sides [v largest-index]
  (reduce + (drop-longest-side v largest-index)))

(defn is-valid-triangle [sum-smallest largest-number]
  (if (> sum-smallest largest-number) 1 0))

(defn get-valid-triangles [input]
  (let [clean-input (clean-and-split-input input)
        column-input (get-column-input clean-input)
        largest-numbers (map get-largest-number column-input)
        largest-number-indexes (map get-index-of-largest-number column-input largest-numbers)
        sum-smallest-sides (map get-sum-of-smallest-triangle-sides column-input largest-number-indexes)]
    (reduce + (map is-valid-triangle sum-smallest-sides largest-numbers))))

(defn -main []
  (let [start (System/nanoTime)
        triangles (get-valid-triangles input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "Number of valid triangles:" triangles)))

;; (-main)
