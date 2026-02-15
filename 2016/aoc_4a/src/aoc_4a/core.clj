(ns aoc-4a.core)
(require 'clojure.string)

;; FOR each room:
;;   SORT letters by 5 most common
;;   IF letters match checksum:
;;     ADD room ID to real room total
;;
;; RETURN real room total

(def test-input "aaaaa-bbb-z-y-x-123[abxyz]
a-b-c-d-e-f-g-h-987[abcde]
not-a-real-room-404[oarel]
totally-real-room-200[decoy]")
(def input-file (slurp "input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/split-lines)))

(defn get-checksums [input]
  (->> input
       (map #(clojure.string/split % #"\["))
       (map second)
       (map #(clojure.string/replace % #"\]" ""))))

(defn get-sector-ids [input]
  (->> input
       (map #(clojure.string/split % #"\["))
       (map first)
       (map #(clojure.string/split % #"\-"))
       (map last)
       (map Integer/parseInt)))

(defn get-encrypted-letters [input]
  (->> input
       (map #(clojure.string/split % #"\["))
       (map first)
       (map #(clojure.string/split % #"\-"))
       (map butlast)
       (map clojure.string/join)))

(defn get-5-most-common-letters [letters]
  (as-> letters l
    (frequencies l)
    (sort-by second > (sort-by first l))
    (take 5 (keys l))
    (clojure.string/join #"" l)))

(defn is-real-room [id common-letters checksum]
  (if (= common-letters checksum) id 0))

(defn get-sum-of-real-room-ids [input]
  (let [clean-input (clean-and-split-input input)
        checksums (get-checksums clean-input)
        sector-ids (get-sector-ids clean-input)
        encrypted-letters (get-encrypted-letters clean-input)
        most-common-letters (map get-5-most-common-letters encrypted-letters)]
    (reduce + (map is-real-room sector-ids most-common-letters checksums))))

(defn -main []
  (let [start (System/nanoTime)
        sum-ids (get-sum-of-real-room-ids input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The sum of real room IDs:" sum-ids)))

;; (-main)
