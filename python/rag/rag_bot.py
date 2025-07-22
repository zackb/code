from llama_index.core.indices.vector_store import VectorStoreIndex
from llama_index.core.readers import SimpleDirectoryReader
from llama_index.llms.ollama import Ollama
from llama_index.embeddings.huggingface import HuggingFaceEmbedding
from llama_index.core import Settings


def main():
    # Load documents
    documents = SimpleDirectoryReader("data", required_exts=[".md"]).load_data()

    # Set up local LLM and embedding model
    # Settings.llm = Ollama(model="mistral")
    Settings.llm = Ollama(model="gemma:2b")
    Settings.embed_model = HuggingFaceEmbedding(model_name="sentence-transformers/all-MiniLM-L6-v2")

    # Build index
    index = VectorStoreIndex.from_documents(documents)

    # Create query engine
    query_engine = index.as_query_engine()

    # Start CLI loop
    print("🔍 Ask me anything (Ctrl+C to quit):")
    while True:
        try:
            question = input("You: ")
            response = query_engine.query(question)
            print("Bot:", response, "\n")
        except KeyboardInterrupt:
            print("\nExiting.")
            break


if __name__ == "__main__":
    main()

