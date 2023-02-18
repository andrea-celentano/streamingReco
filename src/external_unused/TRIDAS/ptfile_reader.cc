#include "../TRIDAS/ptfile_reader.hpp"

namespace tridas { namespace post_trigger {

std::vector< Hit<sample::none> > parse_event(
    std::ifstream& stream,
    int nhits,
    int event_size,
    sample::none)
{
  std::vector< Hit<sample::none> > hits(nhits);

  std::vector< Hit<sample::none> >::iterator current = hits.begin();

  do {
    DataFrameHeader const head = readHeader<DataFrameHeader>(stream);
    event_size -= sizeof(head);

    if (!subsequent(head) && current->m_heads.size() != 0) {
      ++current;
    }

    current->m_heads.push_back(head);

    int const payload_size = getDFHPayloadSize(head);

    stream.ignore(payload_size);
    event_size -= payload_size;

  } while (stream && event_size > 0);

  if (event_size) {
    throw std::runtime_error("Incomplete event in file.");
  }

  return hits;
}

std::vector< Hit<sample::compressed> > parse_event(
    std::ifstream& stream,
    int nhits,
    int event_size,
    sample::compressed)
{
  std::vector< Hit<sample::compressed> > hits(nhits);

  std::vector< Hit<sample::compressed> >::iterator current = hits.begin();

  do {
    DataFrameHeader const head = readHeader<DataFrameHeader>(stream);
    event_size -= sizeof(head);

    if (!compressed(head)) {
      throw std::runtime_error(
          "Error: compressed sample type cannot handle uncompressed samples.");
    }

    if (!subsequent(head) && current->m_heads.size() != 0) {
      ++current;
    }

    current->m_heads.push_back(head);

    std::size_t const nsamples = getDFHNSamples(head);
    std::size_t const payload_size = getDFHPayloadSize(head);

    current->m_samples.reserve(current->m_samples.size() + nsamples);

    event_size -= read_samples<sample::compressed, sample::compressed>(
        stream,
        current->m_samples,
        nsamples);

    std::size_t const diff = payload_size - nsamples * sizeof(sample::compressed);
    stream.ignore(diff);
    event_size -= diff;
  } while (stream && event_size > 0);

  if (event_size) {
    throw std::runtime_error("Incomplete event in file.");
  }

  return hits;
}

std::vector< Hit<sample::uncompressed> > parse_event(
    std::ifstream& stream,
    int nhits,
    int event_size,
    sample::uncompressed)
{
  std::vector< Hit<sample::uncompressed> > hits(nhits);

  std::vector< Hit<sample::uncompressed> >::iterator current = hits.begin();

  do {
    DataFrameHeader const head = readHeader<DataFrameHeader>(stream);
    event_size -= sizeof(head);

    if (!subsequent(head) && current->m_heads.size() != 0) {
      ++current;
    }

    current->m_heads.push_back(head);

    std::size_t const nsamples = getDFHNSamples(head);
    std::size_t const payload_size = getDFHPayloadSize(head);

    current->m_samples.reserve(current->m_samples.size() + nsamples);

    if (!compressed(head)) {
      event_size -= read_samples<sample::uncompressed, sample::uncompressed>(
          stream,
          current->m_samples,
          nsamples);
      std::size_t const diff = payload_size - nsamples * sizeof(sample::uncompressed);
      stream.ignore(diff);
      event_size -= diff;
    } else {
      event_size -= read_samples<sample::compressed, sample::uncompressed>(
          stream,
          current->m_samples,
          nsamples);
      std::size_t const diff = payload_size - nsamples * sizeof(sample::compressed);
      stream.ignore(diff);
      event_size -= diff;
    }
  } while (stream && event_size > 0);

  if (event_size) {
    throw std::runtime_error("Incomplete event in file.");
  }

  return hits;
}

} // ns tridas
} // ns post_trigger
