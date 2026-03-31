(ns aoc-7a.core)
(require 'clojure.string)

;; FOR each line:
;;   IF letters in brackets meet condition:
;;     SKIP line
;;   ELSE IF letters outside brackets meet condition:
;;     INCREMENT counter
;;
;; RETURN counter of IP supported

(def test-input "abba[mnop]qrst
abcd[bddb]xyyx
aaaa[qwer]tyui
ioxxoj[asdfgh]zxcvbn")
(def input-file (slurp "input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/replace #"\r" "")
      (clojure.string/replace #" " "")
      (clojure.string/split #"\n")))

(defn is-abba? [letters]
  (and (char? (first letters)) (= (count letters) 4) (= (first letters) (last letters)) (= (second letters) (nth letters 2)) (not= (first letters) (second letters))))

(defn check-letters-for-abba [words]
  (some true? (mapcat #(map is-abba? (partition-all 4 1 %)) words)))

(defn get-ip-supported [input]
  (let [cleaned-input (clean-and-split-input input)
        split-sequences (map #(clojure.string/split % #"\[|\]") cleaned-input)
        outer-sequences (map #(flatten (partition 1 2 %)) split-sequences)
        inner-sequences (map #(flatten (partition 1 2 (rest %))) split-sequences)
        outer-is-abba? (map check-letters-for-abba outer-sequences)
        inner-is-abba? (map check-letters-for-abba inner-sequences)
        supported-ip (map #(if (true? %1) nil %2) inner-is-abba? outer-is-abba?)]
    (count (remove nil? supported-ip))))

(defn -main []
  (let [start (System/nanoTime)
        output (get-ip-supported input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The output is:" output)))

;; (-main)
