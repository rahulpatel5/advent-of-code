(ns aoc-10a.core)
(require 'clojure.string)
(require 'clojure.set)

;; GET initial state
;; WHILE no bot has the target values:
;;   PROCESS instructions
;;
;; RETURN bot that has the target values

(def test-input "value 5 goes to bot 2
bot 2 gives low to bot 1 and high to bot 0
value 3 goes to bot 1
bot 1 gives low to output 1 and high to bot 0
bot 0 gives low to output 2 and high to output 0
value 2 goes to bot 2")
(def input-file (slurp "input.txt"))
;; choose which input to use
;; (def input test-input)
(def input input-file)

(defn clean-and-split-input [input]
  (-> input
      (clojure.string/replace #"\r" "")
      (clojure.string/split #"\n")))

(defn get-initial-values [goes-lines]
  (reduce #(update %1 (last %2) (fnil conj []) (second %2)) {} goes-lines))

(defn get-bot-number [m target]
  (let [rev-map (clojure.set/map-invert m)]
    (or (get rev-map target) (get rev-map (reverse target)))))

(defn map-has-target? [m target]
  (false? (nil? (get-bot-number m target))))

;; assumes instructions always handle small value then large value
;; assumes there's only one (give) instruction for each bot
(defn update-state [m instructions]
  (let [current-bots (keys (filter #(= (count (val %)) 2) m))
        current-values (map (fn [a] (map #(Integer/parseInt %) a)) (map #(get m %) current-bots))
        small-values (map #(str (reduce min %)) current-values)
        large-values (map #(str (reduce max %)) current-values)
        instruction-lines (map (fn [a] (some #(when (= a (second %)) %) instructions)) current-bots)
        new-bots-small (map #(nth % 6) instruction-lines)
        new-bots-large (map #(last %) instruction-lines)
        updated-map (reduce #(update %1 %2 (constantly [])) m current-bots)
        updated-map (reduce #(if (= (nth (first %2) 5) "bot")
                               (update %1 (second %2) (fnil conj []) (last %2)) %1) updated-map (map vector instruction-lines new-bots-small small-values))
        updated-map (reduce #(if (= (nth (first %2) 10) "bot")
                               (update %1 (second %2) (fnil conj []) (last %2)) %1) updated-map (map vector instruction-lines new-bots-large large-values))]
    updated-map))

(defn recur-get-bot [initial instructions target]
  (loop [m initial] (if (map-has-target? m target) (get-bot-number m target)
                        (recur (update-state m instructions)))))

(defn get-bot [input]
  (let [cleaned-input (clean-and-split-input input)
        target-values ["61" "17"]
        split-words (map #(clojure.string/split % #" ") cleaned-input)
        goes-lines (filter #(= (nth % 2) "goes") split-words)
        initial-values (get-initial-values goes-lines)
        gives-lines (filter #(= (nth % 2) "gives") split-words)]
    (recur-get-bot initial-values gives-lines target-values)))

(defn -main []
  (let [start (System/nanoTime)
        bot (get-bot input)
        end (System/nanoTime)
        elapsed (/ (- end start) 1e9)]
    (println (format "Elapsed: %.6f s" elapsed))
    (println "The bot number is:" bot)))

;; (-main)
