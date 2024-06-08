#ifndef DINAMIC_TEXT_H
#define DINAMIC_TEXT_H

#include <vector>
#include <string>

class IComicText {
public:
	virtual ~IComicText() {}

	virtual size_t getNumLines( ) const = 0;

	virtual void createDinamicText( const std::string &text, size_t anum_lines, float atime_per_letter ) = 0;
	virtual bool update( float deltaTime ) = 0; // Return if it is finished
	virtual const char *getTextOnLine( size_t line ) const = 0;
};

// ---------------------------------------------------------------
class DinamicText : public IComicText {
protected:
	size_t num_lines;
	float time_per_letter;
	float curr_time;
	size_t curr_sheet_id;

	typedef std::vector< std::string > TSheet;
	typedef std::vector< TSheet > TSheets;
	TSheets sheets;

	TSheet current_sheet;

public:
	DinamicText( );
	~DinamicText( );

	size_t getNumLines( ) const { return num_lines; }

	void createDinamicText( const std::string &text, size_t anum_lines, float atime_per_letter );
	bool update( float deltaTime );
	const char *getTextOnLine( size_t line ) const;

private:
	void showWholeSheet( );
	bool isSheetFinished( ) const;
	bool nextSheet( ); // Return if there is no more sheets
};

// ---------------------------------------------------------------
class StaticText : public IComicText {
protected:
	size_t num_lines;
	float curr_time;
	size_t curr_sheet_id;

	typedef std::vector< std::string > TSheet;
	typedef std::vector< TSheet > TSheets;
	TSheets sheets;

	typedef std::vector< float > TTimes;
	TTimes next_sheet_times;

	TSheet *current_sheet;

public:
	StaticText( );
	~StaticText( );

	size_t getNumLines( ) const { return num_lines; }

	void createDinamicText( const std::string &text, size_t anum_lines, float atime_per_letter );
	bool update( float deltaTime );
	const char *getTextOnLine( size_t line ) const;

private:
	bool nextSheet( );
	bool goToNextSheet( );
	void stringSplit(std::string str, std::string delim, std::vector<std::string> &results);
};

#endif //DINAMIC_TEXT_H
