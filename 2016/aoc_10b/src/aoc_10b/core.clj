(ns aoc-10b.core)
(require 'clojure.string)

;; I don't follow exactly what I need to do
;; first get a final output from the puzzle and look at it
;;
;; seems a simple product using the direct output
;;
;; WHILE bot has two values:
;;   PROCESS instructions (transfer values to bot or output)
;;
;; RETURN product of relevant elements of output

(def input-file (slurp "../aoc_10a/input.txt"))
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/replace #"\r" "")
      (clojure.string/split #"\n")))

(defn get-max-size [mp]
  (reduce #(if (> (count %2) %1) (count %2) %1) 0 (vals mp)))

(defn update-values [map-values instructions outputs]
  (let [has-two-values (filter #(= 2 (count (get map-values %))) (keys map-values))
        associated-instructions (mapcat (fn [v] (filter #(= v (second %)) instructions)) has-two-values)
        ;; update bot values
        lower-bots (map #(nth % 6) associated-instructions)
        upper-bots (map #(last %) associated-instructions)
        int-values (map (fn [ns] (map #(Integer/parseInt %) ns)) (map #(get map-values %) has-two-values))
        lower-values (map #(str (reduce min %)) int-values)
        upper-values (map #(str (reduce max %)) int-values)
        updated-values (reduce #(update %1 %2 (constantly [])) map-values has-two-values)
        updated-values (reduce #(if (= (nth (first %2) 5) "bot")
                                  (update %1 (second %2) (fnil conj []) (last %2))
                                  %1)
                               updated-values
                               (map vector associated-instructions lower-bots lower-values))
        updated-values (reduce #(if (= (nth (first %2) 10) "bot")
                                  (update %1 (second %2) (fnil conj []) (last %2))
                                  %1)
                               updated-values
                               (map vector associated-instructions upper-bots upper-values))
        ;; update output values
        updated-outputs (reduce #(if (= (nth (first %2) 5) "output")
                                   (update %1 (second %2) (fnil conj []) (last %2))
                                   %1)
                                outputs
                                (map vector associated-instructions lower-bots lower-values))
        updated-outputs (reduce #(if (= (nth (first %2) 10) "output")
                                   (update %1 (second %2) (fnil conj []) (last %2))
                                   %1)
                                updated-outputs
                                (map vector associated-instructions upper-bots upper-values))]
    [updated-values updated-outputs]))

(defn get-final-output [instructions start-values]
  (loop [vs start-values, os {}] (if (< (get-max-size vs) 2) os
                                     (let [[new-vals new-outputs] (update-values vs instructions os)]
                                       (recur new-vals new-outputs)))))

(defn get-final-product [outputs]
  (let [str-numbers (map #(first (get outputs (str %))) (range 3))
        int-numbers (map #(Integer/parseInt %) str-numbers)]
    (reduce * int-numbers)))

(defn get-output-product [input]
  (let [cleaned-input (clean-and-split-input input)
        split-words (map #(clojure.string/split % #" ") cleaned-input)
        value-lines (filter #(= (first %) "value") split-words)
        starting-values (reduce #(update %1 (last %2) (fnil conj []) (second %2)) {} value-lines)
        give-lines (filter #(= (nth % 2) "gives") split-words)
        final-output (get-final-output give-lines starting-values)]
    (get-final-product final-output)))

(defn -main []
  (let [start (System/nanoTime)
        output (get-output-product input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The product is:" output)))

;; (-main)
