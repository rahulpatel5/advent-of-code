(ns aoc-9a.core)
(require 'clojure.string)

;; FOR each line:
;;   DECOMPRESS line using instructions
;;
;; RETURN sum of lengths of each decompressed line

(def test-input "ADVENT
A(1x5)BC
(3x3)XYZ
A(2x2)BCD(2x2)EFG
(6x1)(1x3)A
X(8x2)(3x3)ABCY")
(def input-file (slurp "input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/replace #"\r" "")
      (clojure.string/replace #" " "")
      (clojure.string/split #"\n")))

(defn split-text [text]
  (clojure.string/split text #"\(|\)" 3))

(defn handle-repeats [text acc]
  (let [[pre-split repeat-info remaining] (split-text text)
        clean-repeat (if (nil? repeat-info) "0x0" (if (nil? remaining) pre-split repeat-info))
        [chr-len reps] (map #(Integer/parseInt %) (clojure.string/split clean-repeat #"x"))
        clean-remaining (if (nil? remaining) (if (nil? repeat-info) "" repeat-info) remaining)
        [to-repeat leftover] [(reduce str "" (take chr-len clean-remaining)) (reduce str "" (drop chr-len clean-remaining))]
        repeated-text (reduce str "" (repeat reps to-repeat))]
    (cond
      (empty? repeat-info) [(reduce str acc pre-split) nil]
      (empty? remaining) [(reduce str acc repeated-text) leftover]
      :else [(reduce #(str %1 %2) acc [pre-split repeated-text]) leftover])))

(defn recurse-process [text]
  (loop [t text, s ""] (if (empty? t) s
                           (let [[processed remaining] (handle-repeats t s)]
                             (recur remaining processed)))))

(defn decompressed-length [input]
  (let [cleaned-input (clean-and-split-input input)
        decompressed-lengths (map recurse-process cleaned-input)]
    (reduce #(+ %1 (count %2)) 0 decompressed-lengths)))

(defn -main []
  (let [start (System/nanoTime)
        length (decompressed-length input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The decompressed length is:" length)))

;; (-main)
