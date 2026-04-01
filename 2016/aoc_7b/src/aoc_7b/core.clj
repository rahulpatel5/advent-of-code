(ns aoc-7b.core)
(require 'clojure.string)

;; repeat solution to part 1, but add changed conditions
;;
;; FOR each line:
;;   IF outer block true AND inner block true:
;;     INCREMENT counter
;;
;; RETURN counter of IPs meeting conditions

(def test-input "aba[bab]xyz
xyx[xyx]xyx
aaa[kek]eke
zazbz[bzb]cdb")
(def input-file (slurp "../aoc_7a/input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/replace #"\r" "")
      (clojure.string/replace #" " "")
      (clojure.string/split #"\n")))

(defn is-aba? [letters]
  (and (char? (first letters)) (= (count letters) 3) (= (first letters) (last letters)) (not= (first letters) (second letters))))

(defn get-letters-for-aba [words]
  (map clojure.string/join (remove nil? (map #(when (true? %1) %2) (mapcat #(map is-aba? (partition-all 3 1 %)) words) (mapcat #(partition-all 3 1 %) words)))))

(defn convert-to-bab [words]
  (map #(str (second %) (first %) (second %)) words))

(defn found-bab? [bab inner-seq]
  (map #(if (= bab %) true nil) inner-seq))

(defn check-babs [babs inner-abas]
  (if (or (empty? babs) (empty? inner-abas)) nil (some true? (mapcat #(found-bab? % inner-abas) babs))))

(defn get-ip-supported [input]
  (let [cleaned-input (clean-and-split-input input)
        split-sequences (map #(clojure.string/split % #"\[|\]") cleaned-input)
        outer-sequences (map #(flatten (partition 1 2 %)) split-sequences)
        inner-sequences (map #(flatten (partition 1 2 (rest %))) split-sequences)
        outer-aba-sequences (map get-letters-for-aba outer-sequences)
        bab-sequences (map convert-to-bab outer-aba-sequences)
        inner-aba-sequences (map get-letters-for-aba inner-sequences)
        supported-ip (map check-babs bab-sequences inner-aba-sequences)]
    (count (remove nil? supported-ip))))

(defn -main []
  (let [start (System/nanoTime)
        output (get-ip-supported input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The output is:" output)))

;; (-main)
