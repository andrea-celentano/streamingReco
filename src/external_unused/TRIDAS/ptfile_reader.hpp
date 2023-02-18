#ifndef TRIDAS_TOOLS_PT_FILE_READER_HPP
#define TRIDAS_TOOLS_PT_FILE_READER_HPP

#include <fstream>
#include <string>
#include <stdexcept>

#include "../TRIDAS/tridas_dataformat.hpp"

namespace {

template<typename Header>
Header readHeader(std::ifstream& input) {
	Header head;
	input.read(static_cast<char*>(static_cast<void*>(&head)), sizeof(head));

	if (input.gcount() != sizeof(head)) {
		throw std::runtime_error("Error reading the full header from file.");
	}

	return head;
}

inline std::string readDatacard(std::ifstream& input, std::size_t dc_size) {
	input.seekg(sizeof(PTHeader));

	std::string content(dc_size, '\0');

	input.read(&content[0], content.size());

	return content;
}

template<typename SampleData, typename SampleProg>
int read_samples(std::ifstream& stream, std::vector<SampleProg>& samples, int nsamples) {
	int read_size = 0;
	for (int i = 0; i < nsamples; ++i) {
		SampleData s = 0;
		stream.read(static_cast<char*>(static_cast<void*>(&s)), sizeof(s));

		samples.push_back(s);

		read_size += stream.gcount();
	}
	return read_size;
}

} // anon-ns

namespace tridas {
namespace post_trigger {

// forward declarations
template<typename Sample>
class PtFileReader;

template<typename Sample>
class TimeSlice;

template<typename Sample>
class Event;

template<typename Sample>
class Hit;

namespace sample {

typedef uint16_t uncompressed;
typedef uint8_t compressed;
typedef struct {
} none;

} // ns sample

std::vector<Hit<sample::uncompressed> > parse_event(std::ifstream& stream, int nhits, int event_size, sample::uncompressed);

std::vector<Hit<sample::compressed> > parse_event(std::ifstream& stream, int nhits, int event_size, sample::compressed);

std::vector<Hit<sample::none> > parse_event(std::ifstream& stream, int nhits, int event_size, sample::none);

template<typename Sample>
class Hit {
	template<typename S>
	friend class Event;

	friend std::vector<Hit<sample::uncompressed> > parse_event(std::ifstream& stream, int nhits, int event_size, sample::uncompressed);

	friend std::vector<Hit<sample::compressed> > parse_event(std::ifstream& stream, int nhits, int event_size, sample::compressed);

	friend std::vector<Hit<sample::none> > parse_event(std::ifstream& stream, int nhits, int event_size, sample::none);

	std::vector<DataFrameHeader> m_heads;
	std::vector<Sample> m_samples;

public:

	Hit() {
	}

	Hit(Hit const& copy) {
		*this = copy;
	}

	Hit& operator =(Hit const& copy) {
		if (this != &copy) {
			m_heads = copy.m_heads;
			m_samples = copy.m_samples;
		}
		return *this;
	}

	// Access to header members

	std::size_t nFrames() const {
		return m_heads.size();
	}

	DataFrameHeader& frameHeader(int n) {
		return m_heads[n];
	}

	DataFrameHeader const& frameHeader(int n) const {
		return m_heads[n];
	}

	// Iteration over Samples

	Sample& operator [](std::size_t n) {
		return m_samples[n];
	}

	Sample const& operator [](std::size_t n) const {
		return m_samples[n];
	}

	typedef typename std::vector<Sample>::iterator iterator;
	typedef typename std::vector<Sample>::const_iterator const_iterator;

	iterator begin() {
		return m_samples.begin();
	}

	iterator end() {
		return m_samples.end();
	}

	const_iterator begin() const {
		return m_samples.begin();
	}

	const_iterator end() const {
		return m_samples.end();
	}
};

template<typename Sample>
class Event {
	template<typename S>
	friend class TimeSlice;

	TEHeaderInfo m_head;
	std::vector<Hit<Sample> > m_hits;

	Event(std::ifstream& stream) :
			m_head(readHeader<TEHeaderInfo>(stream)), m_hits(parse_event(stream, nHit(), size() - sizeof(m_head), Sample())) {
	}

public:

	Event(Event const& copy) {
		*this = copy;
	}

	Event& operator =(Event const& copy) {
		if (this != &copy) {
			m_head = copy.m_head;
			m_hits = copy.m_hits;
		}
		return *this;
	}

	const static int max_triggers_number = MAX_TRIGGERS_NUMBER;
	const static int max_plugins_number = MAX_PLUGINS_NUMBER;

	// Access to header members

	uint32_t eventTag() const {
		return m_head.EventTag;
	}

	uint32_t id() const {
		return m_head.EventID;
	}

	uint32_t size() const {
		return m_head.EventL;
	}

	uint32_t nHit() const {
		return m_head.nHit;
	}

	fine_time startTime() const {
		return fine_time(m_head.StartTime);
	}

	bool tsComplete() const {
		return m_head.TSCompleted;
	}

	uint32_t nseeds(std::size_t n) const {
		assert(n < max_triggers_number);
		return m_head.nseeds[n];
	}

	uint32_t plugin_trigtype(std::size_t n) const {
		assert(n < max_plugins_number);
		return m_head.plugin_trigtype[n];
	}

	uint32_t plugin_nseeds(std::size_t n) const {
		assert(n < max_plugins_number);
		return m_head.plugin_nseeds[n];
	}

	// Iteration over Hits

	Hit<Sample>& operator [](std::size_t n) {
		return m_hits[n];
	}

	Hit<Sample> const& operator [](std::size_t n) const {
		return m_hits[n];
	}

	typedef typename std::vector<Hit<Sample> >::iterator iterator;
	typedef typename std::vector<Hit<Sample> >::const_iterator const_iterator;

	iterator begin() {
		return m_hits.begin();
	}

	iterator end() {
		return m_hits.end();
	}

	const_iterator begin() const {
		return m_hits.begin();
	}

	const_iterator end() const {
		return m_hits.end();
	}
};

template<typename Sample>
class TimeSlice {
	template<typename S>
	friend class PtFileReader;

	TimeSliceHeader m_head;
	std::vector<Event<Sample> > m_events;

	TimeSlice(std::ifstream& stream) :
			m_head(readHeader<TimeSliceHeader>(stream)) {
		m_events.reserve(nEvents());
		for (int i = 0; i < nEvents(); ++i) {
			m_events.push_back(Event<Sample>(stream));
		}
	}

public:

	TimeSlice(TimeSlice const& copy) {
		*this = copy;
	}

	TimeSlice& operator =(TimeSlice const& copy) {
		if (this != &copy) {
			m_head = copy.m_head;
			m_events = copy.m_events;
		}
		return *this;
	}

	// Access to header members

	TS_t id() const {
		return m_head.TS_ID;
	}

	int nEvents() const {
		return m_head.NEvents;
	}

	int size() const {
		return m_head.TS_size;
	}

	// Iteration over Events

	Event<Sample>& operator [](std::size_t n) {
		return m_events[n];
	}

	Event<Sample> const& operator [](std::size_t n) const {
		return m_events[n];
	}

	typedef typename std::vector<Event<Sample> >::iterator iterator;
	typedef typename std::vector<Event<Sample> >::const_iterator const_iterator;

	iterator begin() {
		return m_events.begin();
	}

	iterator end() {
		return m_events.end();
	}

	const_iterator begin() const {
		return m_events.begin();
	}

	const_iterator end() const {
		return m_events.end();
	}
};

template<typename Sample>
class PtFileReader {
	// non-copyable
	PtFileReader(PtFileReader const&);
	PtFileReader& operator =(PtFileReader const&);

	std::ifstream mutable m_file;
	PTHeader const m_head;
	std::vector<std::size_t> m_ts_pos;

public:

	explicit PtFileReader(std::string const& path) :
			m_file(path.c_str()), m_head(readHeader<PTHeader>(m_file)) {
		m_file.seekg(datacardSize(), std::ios::cur);

		m_ts_pos.reserve(nTS());

		do {
			m_ts_pos.push_back(m_file.tellg());

			TimeSliceHeader const ts_head = readHeader<TimeSliceHeader>(m_file);

			m_file.ignore(ts_head.TS_size - sizeof(ts_head));
		} while (m_file && m_ts_pos.size() < nTS());

		m_file.seekg(0);
		m_file.clear();
	}
	
	std::size_t tellg(void){ return m_file.tellg(); }

	// Access to header members

	uint32_t runNumber() const {
		return m_head.run_number;
	}

	uint32_t fileNumber() const {
		return m_head.file_number;
	}

	uint64_t runStartTime() const {
		return m_head.run_start_time;
	}

	uint32_t datacardSize() const {
		return m_head.datacard_size;
	}

	uint32_t nEvents() const {
		return m_head.number_of_events;
	}

	uint32_t nTS() const {
		return m_head.number_of_timeslices;
	}

	uint32_t fileSize() const {
		return m_head.effective_file_size;
	}

	std::string datacard() const {
		return readDatacard(m_file, datacardSize());
	}

	// Iteration over TimeSlices

	TimeSlice<Sample> operator [](std::size_t pos) const {
		m_file.seekg(m_ts_pos[pos]);
		m_file.clear();
		return TimeSlice<Sample>(m_file);
	}

	class iterator {
		friend class PtFileReader;

		PtFileReader* m_ptfilereader;
		std::size_t m_pos;

		iterator(PtFileReader* reader, std::size_t pos) :
				m_ptfilereader(reader), m_pos(pos) {
		}

	public:

		iterator() :
				m_ptfilereader(0), m_pos(0) {
		}

		iterator(iterator const& copy) :
				m_ptfilereader(copy.m_ptfilereader), m_pos(copy.m_pos) {
		}

		iterator& operator =(iterator const& copy) {
			if (this != &copy) {
				m_ptfilereader = copy.m_ptfilereader;
				m_pos = copy.m_pos;
			}
			return *this;
		}

		bool operator !=(iterator const& second) const {
			return !(*this == second);
		}

		bool operator ==(iterator const& second) const {
			return m_pos == second.m_pos && m_ptfilereader == second.m_ptfilereader;
		}

		TimeSlice<Sample> operator *() const {
			return m_ptfilereader->operator [](m_pos);
		}

		iterator operator ++(int) {
			iterator copy(*this);
			++m_pos;
			return copy;
		}

		iterator& operator ++() {
			++m_pos;
			return *this;
		}

		iterator operator --(int) {
			iterator copy(*this);
			--m_pos;
			return copy;
		}

		iterator& operator --() {
			--m_pos;
			return *this;
		}

		iterator operator +(int n) const {
			return iterator(m_ptfilereader, m_pos + n);
		}

		iterator operator -(int n) const {
			return iterator(m_ptfilereader, m_pos - n);
		}

		iterator& operator +=(int n) {
			m_pos += n;
			return *this;
		}

		iterator& operator -=(int n) {
			m_pos -= n;
			return *this;
		}
	};

	class const_iterator {
		friend class PtFileReader;

		PtFileReader const* m_ptfilereader;
		std::size_t m_pos;

		const_iterator(PtFileReader const* reader, std::size_t pos) :
				m_ptfilereader(reader), m_pos(pos) {
		}

	public:

		const_iterator() :
				m_ptfilereader(0), m_pos(0) {
		}

		const_iterator(const_iterator const& copy) :
				m_ptfilereader(copy.m_ptfilereader), m_pos(copy.m_pos) {
		}

		const_iterator& operator =(const_iterator const& copy) {
			if (this != &copy) {
				m_ptfilereader = copy.m_ptfilereader;
				m_pos = copy.m_pos;
			}
			return *this;
		}

		bool operator !=(const_iterator const& second) const {
			return !(*this == second);
		}

		bool operator ==(const_iterator const& second) const {
			return m_pos == second.m_pos && m_ptfilereader == second.m_ptfilereader;
		}

		TimeSlice<Sample> operator *() const {
			return m_ptfilereader->operator [](m_pos);
		}

		const_iterator operator ++(int) {
			const_iterator copy(*this);
			++m_pos;
			return copy;
		}

		const_iterator& operator ++() {
			++m_pos;
			return *this;
		}

		const_iterator operator --(int) {
			const_iterator copy(*this);
			--m_pos;
			return copy;
		}

		const_iterator& operator --() {
			--m_pos;
			return *this;
		}

		const_iterator operator +(int n) const {
			return const_iterator(m_ptfilereader, m_pos + n);
		}

		const_iterator operator -(int n) const {
			return const_iterator(m_ptfilereader, m_pos - n);
		}

		const_iterator& operator +=(int n) {
			m_pos += n;
			return *this;
		}

		const_iterator& operator -=(int n) {
			m_pos -= n;
			return *this;
		}
	};

	iterator begin() {
		return iterator(this, 0);
	}

	iterator end() {
		return iterator(this, m_ts_pos.size());
	}

	const_iterator begin() const {
		return const_iterator(this, 0);
	}

	const_iterator end() const {
		return const_iterator(this, m_ts_pos.size());
	}
};

} // ns post_trigger
} // ns tridas

#endif // TRIDAS_TOOLS_PT_FILE_READER_HPP
