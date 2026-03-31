(ns aoc-6a.core)
(require 'clojure.string)

;; FOR each character position:
;;   FIND most frequent letter
;;
;; RETURN most common letter in each position

(def test-input "eedadn
drvtee
eandsr
raavrd
atevrs
tsrnev
sdttsa
rasrtv
nssdts
ntnada
svetve
tesnvt
vntsnd
vrdear
dvrsen
enarar")
(def input-file (slurp "input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/replace #"\r" "")
      (clojure.string/replace #" " "")
      (clojure.string/split #"\n")))

(defn split-by-position [col pos]
  (map #(nth % pos) col))

(defn get-message [input]
  (let [cleaned-input (clean-and-split-input input)
        char-sequences (map #(clojure.string/split % #"") cleaned-input)
        word-length (count (first cleaned-input))
        letters-by-position (map #(split-by-position char-sequences %) (range word-length))
        char-frequencies (map frequencies letters-by-position)
        sorted-frequencies (map #(sort-by val %) char-frequencies)
        most-common-char (map last sorted-frequencies)]
    (reduce str (map key most-common-char))))

(defn -main []
  (let [start (System/nanoTime)
        message (get-message input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The message is:" message)))

;; (-main)
