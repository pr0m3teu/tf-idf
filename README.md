# tf-idf

A small command-line document search tool written in C. It indexes all files in a given
folder, computes TF-IDF (term frequency–inverse document frequency) scores, and lets you
interactively search for terms to find the most relevant documents.

## How it works

- **Indexing**: every regular file in the target folder is read, tokenized on whitespace
  and punctuation, lowercased, and stored as a per-document term-frequency hash map
  (separate chaining, sum-of-bytes hash, `M = 50` buckets).
- **Search**: for a query term, `tf_idf()` combines the term frequency within a document
  with the inverse document frequency across the whole corpus, and the documents with the
  highest scores are reported (up to 10).
- **Data structures**:
  - `Term_Freq_List`: singly linked list node for a hash bucket (`term`, `freq`).
  - `Doc`: a document's name, term count, and its term-frequency hash table.
  - `da`: a dynamic array of `Doc*`, used to hold all indexed documents.

## Build

```sh
make
```

Requires `gcc`. Produces a `main` executable.

## Usage

```sh
./main <FOLDER>
```

The program indexes every regular file directly inside `<FOLDER>`, then prompts:

```
Search term > <your query>
> Searching for: "<your query>"
> Found N relevant files:
Doc 0: <path>
...
Continue (yes: y, no: n):
```

Type `y` to run another search, anything else to exit.

## Notes / known limitations

- `hash()` sums `unsigned char` values mod `M`; collisions are handled via chaining, but
  `M = 50` is small for large corpora.
- Only top-level regular files in `<FOLDER>` are indexed (no recursion into subdirectories).
- No `-Werror`; build with `-Wall -Wextra` as configured in the `Makefile`.
