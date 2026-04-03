(ns aoc-8b.core)
(require 'clojure.string)

;; repeat solution to part 1
;; print final output
;;
;; there may be a way to convert the output into text
;; but that seems needlessly complicated

(def input-file (slurp "../aoc_8a/input.txt"))
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/replace #"\r" "")
      (clojure.string/split #"\n")))

(defn set-up-screen [size]
  (vec (repeat (second size) (vec (repeat (first size) 0)))))

(defn get-coordinates [col row]
  (map #(vector %1 %2) (flatten (repeat (range row))) (mapcat #(repeat row %) (range col))))

;; assumes rectangle isn't larger than screen
(defn apply-rect [screen col row]
  (vec (reduce #(assoc-in %1 %2 1) screen (get-coordinates col row))))

(defn get-rect-args [shape screen]
  (let [shp (clojure.string/split shape #"x")
        col (Integer/parseInt (first shp))
        row (Integer/parseInt (second shp))]
    (apply-rect screen col row)))

;; assumes shift positive and is less than element length
(defn shifted-element [element shift]
  (take (count element) (drop (- (count element) shift) (cycle element))))

(defn rotate-row [screen row shift]
  (assoc screen row (vec (shifted-element (get screen row) shift))))

(defn get-col-coord [screen col]
  (map #(vector % col) (range (count screen))))

(defn get-col-element [screen col]
  (map #(get-in screen %) (get-col-coord screen col)))

(defn rotate-col [screen col shift element]
  (reduce #(assoc-in %1 (first %2) (second %2)) screen (map vector (get-col-coord screen col) (shifted-element element shift))))

(defn get-rot-args [index shift screen]
  (let [inf (clojure.string/split index #"=")
        dir (first inf)
        idx (Integer/parseInt (second inf))
        sft (Integer/parseInt shift)]
    (case dir
      "x" (rotate-col screen idx sft (get-col-element screen idx))
      "y" (rotate-row screen idx sft)
      (throw (Exception. "Did not get valid rotation instruction.")))))

(defn apply-instructions [screen instruction]
  (let [[a b c _ e] (clojure.string/split instruction #" ")]
    (cond
      (= a "rect") (get-rect-args b screen)
      (= a "rotate") (get-rot-args c e screen)
      :else (throw (Exception. "Received unknown instruction.")))))

(defn get-lit-pixels [input]
  (let [cleaned-input (clean-and-split-input input)
        screen-size [50 6]
        screen (set-up-screen screen-size)
        final-screen (reduce apply-instructions screen cleaned-input)
        ;; based on this code: https://stackoverflow.com/a/41147810
        final-text (mapv #(replace {0 "."} %) final-screen)]
    (run! println final-text)))

(defn -main []
  (let [start (System/nanoTime)
        _ (get-lit-pixels input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))))

;; (-main)
