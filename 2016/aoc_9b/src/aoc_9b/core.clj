(ns aoc-9b.core)
(require 'clojure.string)

;; change solution to part 1 to multiply multipliers
;;
;; FOR each line:
;;   IDENTIFY which multipliers to multiply
;;   GET length of decompressed line
;;
;; RETURN total lengths of decompressed lines
;;
;; see if counting this directly causes issues
;; might be possible to focus only on multipliers?
;; the text itself is irrelevant
;;
;; idea: associate a multiplier with each character
;; this starts at 1
;; multiply by the multiplier using multiplier range
;; set the multiplier for characters within brackets to 0
;; then sum the multipliers for the line
;;
;; assumes that digits only happen inside brackets
;; assumes that only lowercase character is 'x'

(def test-input "(3x3)XYZ
X(8x2)(3x3)ABCY
(27x12)(20x12)(13x14)(7x10)(1x12)A
(25x3)(3x3)ABC(2x3)XY(5x2)PQRSTX(18x9)(3x2)TWO(5x7)SEVEN")
(def input-file (slurp "../aoc_9a/input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/replace #"\r" "")
      (clojure.string/replace #" " "")
      (clojure.string/split #"\n")))

(defn get-repeat-info [text]
  (let [split-text (clojure.string/split text #"\(|\)")
        filtered-text (filter #(some #{\x} (seq %)) split-text)
        info-in-str (map #(clojure.string/split % #"x") filtered-text)]
    (map (fn [a] (map #(Integer/parseInt %) a)) info-in-str)))

(defn get-start-positions [text]
  (let [positions (range (count text))
        bracket-positions (map #(= \) %) (seq text))]
    (remove nil? (map #(when (true? %1) (inc %2)) bracket-positions positions))))

(defn get-multipliers-for-line [start-position [span multiplier] text-length]
  (into [] (concat (into [] (repeat start-position 1))
                   (into [] (repeat span multiplier))
                   (into [] (repeat (- text-length (+ start-position span)) 1)))))

(defn is-digit? [chr]
  (and (>= (int chr) (int \0)) (<= (int chr) (int \9))))

(defn clean-multiplier [text multiplier]
  (map #(if (or (= \( %1)
                (= \) %1)
                (is-digit? %1)
                (= \x %1)) 0 %2) text multiplier))

(defn get-multipliers [text]
  (let [identity-multipliers (repeat (count text) 1)
        positions (range (count text))
        repeat-info (get-repeat-info text)
        start-positions (get-start-positions text)
        multipliers (map #(get-multipliers-for-line %1 %2 (count text)) start-positions repeat-info)
        cleaned-multipliers (map #(clean-multiplier text %) multipliers)]
    (reduce (fn [a b] (mapv * a b)) cleaned-multipliers)))

(defn decompressed-length [input]
  (let [cleaned-input (clean-and-split-input input)
        multipliers (map get-multipliers cleaned-input)]
    (reduce + (mapcat identity multipliers))))

(defn -main []
  (let [start (System/nanoTime)
        length (decompressed-length input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The decompressed length is:" length)))

;; (-main)
