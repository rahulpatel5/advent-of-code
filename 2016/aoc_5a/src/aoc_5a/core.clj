(ns aoc-5a.core
  (:import (java.security MessageDigest)))
(require 'clojure.string)

;; SET UP md5 checksum
;; WHILE password less than full number of characters:
;;   FOR number in increasing range:
;;     IF checksum starts with 5 zeros:
;;       ADD 6th character to password
;;
;; RETURN password
;;
;; this seems to take a lot of time to run
;; is there a more efficient or better way to do this?

(def test-input "abc")
(def input-file (slurp "input.txt"))
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

(defn loop-hash-for-password [s num-chars]
  (let [non-negative-integers (range)
        string-integers (map #(str s %) non-negative-integers)
        byte-inputs (map #(.getBytes % "UTF-8") string-integers)
        checksums (map get-checksum byte-inputs)
        hex-values (map convert-bytes-to-hex checksums)
        initial-chars (map #(take 5 %) hex-values)
        valid-values (map #(= [\0 \0 \0 \0 \0] %) initial-chars)
        password-elements (map #(get % 5) hex-values)
        valid-password-elements (map #(when (true? %2) %1) password-elements valid-values)
        password (take num-chars (remove nil? valid-password-elements))]
    (reduce str password)))

(defn get-password [input]
  (-> input
      clean-and-split-input
      ;; get 8 characters for password
      (loop-hash-for-password 8)))

(defn -main []
  (let [start (System/nanoTime)
        output (get-password input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The output is:" output)))

;; (-main)
