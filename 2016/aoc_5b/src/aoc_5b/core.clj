(ns aoc-5b.core
  (:import (java.security MessageDigest)))
(require 'clojure.string)

;; repeat solution to part 1
;; take first element for each position
;; should be able to use lazy sequences for this too
;; interested in how to do a faster solution
;;
;; SET UP md5 checksum
;; GET checksums that meet criteria
;; GET 6th character for checksums meeting criteria
;; GET 7th character for checksums meeting criteria
;; FOR first instance of 7th character:
;;   ADD 6th character in the relevant position
;;
;; RETURN password

(def test-input "abc")
(def input-file (slurp "../aoc_5a/input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/replace #"\n" "")
      clojure.string/trim))

;; following two functions use the code from this page:
;; https://clojurepatterns.com/8/1/18/
(defn get-checksum [byte-input]
  (let [md (MessageDigest/getInstance "MD5")]
    (.digest md byte-input)))

(defn convert-bytes-to-hex [bytes]
  (apply str (map (fn [b] (format "%02x" b)) bytes)))

(defn is-position-in-range? [chr num-chars]
  (and (< (- (int chr) 48) num-chars) (>= (- (int chr) 48) 0)))

(defn get-element-for-position [element-position-mapped pos]
  (filter #(= (char (+ pos 48)) (second %)) element-position-mapped))

(defn loop-hash-for-password [s num-chars]
  (let [non-negative-integers (range)
        string-integers (map #(str s %) non-negative-integers)
        byte-inputs (map #(.getBytes % "UTF-8") string-integers)
        checksums (map get-checksum byte-inputs)
        hex-values (map convert-bytes-to-hex checksums)
        initial-chars (map #(take 5 %) hex-values)
        valid-values (map #(= [\0 \0 \0 \0 \0] %) initial-chars)
        password-positions (map #(get % 5) hex-values)
        valid-positions (map #(when (is-position-in-range? % num-chars) %) password-positions)
        password-elements (map #(get % 6) hex-values)
        valid-password-positions (map #(when (true? %2) %1) valid-positions valid-values)
        valid-password-elements (map #(when (true? %2) %1) password-elements valid-values)
        element-position-mapped (map #(vector %1 %2) valid-password-elements valid-password-positions)
        elements-by-position (map #(get-element-for-position element-position-mapped %) (range num-chars))
        password (map first (map first elements-by-position))]
    (reduce str password)))

(defn get-password [input]
  (-> input
      clean-and-split-input
      ;; get 8 characters for password
      (loop-hash-for-password 8)))

(defn -main []
  (let [start (System/nanoTime)
        password (get-password input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The password is:" password)))

;; (-main)
