(ns aoc-4b.core)
(require 'clojure.string)

;; re-use part 1 solution
;; don't understand what the puzzle is here
;; may need to look at the output and see what it says
;;
;; FOR each room:
;;   IF a real room [use part 1 solution]:
;;     DECRYPT name
;;     IF name contains "northpole":
;;       RETURN room
;;
;; not sure if finding real rooms was a necessary first step
;;
;; might be a shortcut to getting the room, by encrypting answer
;; but the puzzle might have accounted for that?

(def test-input "qzmt-zixmtkozy-ivhz-343")
(def input-file (slurp "../aoc_4a/input.txt"))
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

(defn get-room-ids [input]
  (->> input
       (map #(clojure.string/split % #"\["))
       (map first)
       (map #(clojure.string/split % #"\-"))
       (map last)
       (map Integer/parseInt)))

(defn get-room-letters-hyphens-split [input]
  (->> input
       (map #(clojure.string/split % #"\["))
       (map first)
       (map #(clojure.string/split % #"\-"))
       (map butlast)
       (map #(clojure.string/join "-" %))))

(defn get-room-letters-clean [hyphen-letters]
  (->> hyphen-letters
       (map #(clojure.string/split % #"\-"))
       (map clojure.string/join)))

(defn get-5-most-common-letters [letters]
  (as-> letters l
    (frequencies l)
    (sort-by second > (sort-by first l))
    (keys l)
    (take 5 l)
    (clojure.string/join #"" l)))

(defn find-real-rooms [common-letters checksum]
  (if (= common-letters checksum) true nil))

(defn convert-to-unicode [letter-seq]
  (map (fn [s] (map int s)) letter-seq))

(defn convert-to-str [unicode]
  (as-> unicode u
    (map char u)
    (map str u)
    (clojure.string/join u)))

(defn adjust-unicode [unicode]
  (let [a (int (first "a"))
        unicode-relative-to-a (map (partial map (partial + (- a))) unicode)
        mod-unicode (map (partial map #(mod % 26)) unicode-relative-to-a)]
    (map (partial map (partial + a)) mod-unicode)))

(defn get-decrypted-letters [letters cypher]
  (let [hyphen-split-letters (clojure.string/split letters #"\-")
        unicode-numbers (convert-to-unicode hyphen-split-letters)
        unicode-plus-cypher (map (partial map (partial + cypher)) unicode-numbers)
        adjusted-unicode-numbers (adjust-unicode unicode-plus-cypher)
        decrypted-letters (map convert-to-str adjusted-unicode-numbers)]
    (clojure.string/join #" " decrypted-letters)))

(defn find-room [decrypted regex]
  (map #(re-find regex %) decrypted))

(defn get-room-for-north-pole-objects [input]
  (let [clean-input (clean-and-split-input input)
        checksums (get-checksums clean-input)
        cyphers (get-room-ids clean-input)
        letters-hyphen-split (get-room-letters-hyphens-split clean-input)
        encrypted-letters (get-room-letters-clean letters-hyphen-split)
        most-common-letters (map get-5-most-common-letters encrypted-letters)
        room-status (map find-real-rooms most-common-letters checksums)
        decrypted-letters (map get-decrypted-letters letters-hyphen-split cyphers)
        decrypted-for-real-rooms (map second (filter first (map vector room-status decrypted-letters)))
        cyphers-for-real-rooms (map second (filter first (map vector room-status cyphers)))
        the-room-we-want (find-room decrypted-for-real-rooms #"northpole")]
    (map second (filter first (map vector the-room-we-want cyphers-for-real-rooms)))))

(defn -main []
  (let [start (System/nanoTime)
        id (first (get-room-for-north-pole-objects input))
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The relevant room ID:" id)))

;; (-main)
