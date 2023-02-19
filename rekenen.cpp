
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <vector>
#include <exception>
#include <iomanip>

//clear && gcc -o reken rekenen.cpp --std=c++20

enum class strategie { altijd2Verdedigen, altijd1Verdedigen, besteKansVerdedigen };

/*
float kans(int verdedigers, int aanvallen[3])
{

}

///Berekent hoeveel dobbelstenen de verdediger het beste kan gebruiken
int besteKansPerAanval(int aanvallen[3])
{

}*/

struct worpAanval
{
	worpAanval(int gooi0, int gooi1 = 0, int gooi2 = 0)
	{
		stenen[0] = gooi0;
		stenen[1] = gooi1;
		stenen[2] = gooi2;

		aantalStenen = (gooi0 != 0) + (gooi1 != 0) + (gooi2 != 0);

		assert(gooi0 < 7 && gooi1 < 7 && gooi2 < 7);

		std::sort(std::begin(stenen), std::end(stenen), std::greater<int>());
	}

	int stenen[3], aantalStenen;

	int operator[](size_t i) const 	{ return stenen[i]; } 

	bool operator<(const worpAanval & rechts) const 
	{
		return 	 stenen[0]  < rechts.stenen[0] 	|| 
				(stenen[0] == rechts.stenen[0] 	&& stenen[1]  < rechts.stenen[1]) 	|| 
				(stenen[0] == rechts.stenen[0] 	&& stenen[1] == rechts.stenen[1] 	&& stenen[2] < rechts.stenen[2]);
	}

	bool operator==(const worpAanval & rechts) const 
	{
		for(size_t i=0; i<3; i++)
			if(stenen[i] != rechts.stenen[i])
				return false;
		return true;
	}

	std::string str() const {
		return "aanval: " +	 std::to_string(stenen[0]) + (aantalStenen > 1 ? ", " + std::to_string(stenen[1]) : "") + (aantalStenen > 2 ? ", " + std::to_string(stenen[2]) : "");
	}
};

struct worpVerdediging
{

	worpVerdediging(int gooi0, int gooi1 = 0)
	{
		stenen[0] = gooi0;
		stenen[1] = gooi1;

		aantalStenen = (gooi0 != 0) + (gooi1 != 0);

		assert(gooi0 < 7 && gooi1 < 7);

		std::sort(std::begin(stenen), std::end(stenen), std::greater<int>());
	}

	int stenen[2], aantalStenen;

	int operator[](size_t i) const 	{ return stenen[i]; } 

	bool operator<(const worpVerdediging & rechts) const 
	{
		return 	 stenen[0]  < rechts.stenen[0] 	|| 
				(stenen[0] == rechts.stenen[0] 	&& stenen[1]  < rechts.stenen[1]);
	}

	bool operator==(const worpVerdediging & rechts) const
	{
		for(size_t i=0; i<2; i++)
			if(stenen[i] != rechts.stenen[i])
				return false;
		return true;
	}

	std::string str() const {
		return "verdediging: " + std::to_string(stenen[0]) + (aantalStenen > 1 ? ", " + std::to_string(stenen[1]) : "");
	}
};

struct slag
{
	slag(worpAanval aanval, worpVerdediging verdediging)
	: aanval(aanval), verdediging(verdediging)
	{
		//std::cout << "Nieuwe slag en " << aanval.str() << " terwijl " << verdediging.str() << ".\t";

		dodeAanvallers 	= berekenDodeAanvallers();
		dodeVerdedigers = berekenDodeVerdedigers();

		//std::cout << "dode aanvallers: " << dodeAanvallers << " en dode verdedigers: " << dodeVerdedigers << std::endl;
	}

	bool operator<(const slag & rechts) const 
	{
		return 	 aanval  < rechts.aanval 	|| 
				(aanval == rechts.aanval 	&& verdediging < rechts.verdediging);
	}

	bool operator==(const slag & rechts) const { return aanval == rechts.aanval && verdediging == rechts.verdediging; }

	int berekenDodeAanvallers()
	{
		switch(aanval.aantalStenen)
		{
		case 1:
			return !(aanval[0] > verdediging[0]); //want gesorteerd
			
		case 2:
		case 3:
			return !(aanval[0] > verdediging[0]) + (!(aanval[1] > verdediging[1]) && verdediging.aantalStenen == 2);
		}

		std::cout << "aanval.aantalStenen is: " << aanval.aantalStenen << std::endl;
		throw std::runtime_error("berekenDodeAanvallers???");
	}

	int berekenDodeVerdedigers()
	{
		switch(aanval.aantalStenen)
		{
		case 1:
			return (aanval[0] > verdediging[0]); //want gesorteerd
			
		case 2:
		case 3:
			return (aanval[0] > verdediging[0]) + (aanval[1] > verdediging[1] && verdediging.aantalStenen == 2);
		}

		std::cout << "verdediging.aantalStenen is: " << verdediging.aantalStenen << std::endl;
		throw std::runtime_error("berekenDodeVerdedigers???");
	}

	worpAanval		aanval;
	worpVerdediging verdediging;
	int dodeAanvallers, dodeVerdedigers;

};

struct mogelijkeSlag
{
	mogelijkeSlag(int aanvalsDobbels, int verdedigingsDobbels)
	: aanvalsDobbels(aanvalsDobbels), verdedigingsDobbels(verdedigingsDobbels)
	{
		std::vector<worpAanval> 		aanvallen;
		std::vector<worpVerdediging>	verdedigingen;
		std::vector<slag>				slagen;

		for(size_t a1=1; a1<7; a1++)
			for(size_t a2=aanvalsDobbels < 2 ? 0 : 1; a2< (aanvalsDobbels < 2 ? 1 : 7); a2++)
				for(size_t a3=aanvalsDobbels < 3 ? 0 : 1; a3< (aanvalsDobbels < 3 ? 1 : 7); a3++)
					aanvallen.push_back(worpAanval(a1, a2, a3));

		for(size_t v1=1; v1<7; v1++)
			for(size_t v2=verdedigingsDobbels < 2 ? 0 : 1; v2< (verdedigingsDobbels < 2 ? 1 : 7); v2++)
					verdedigingen.push_back(worpVerdediging(v1, v2));

		for(worpAanval & aanval : aanvallen)
			for(worpVerdediging & verdediging : verdedigingen)
				slagen.push_back(slag(aanval, verdediging));

		for(slag & slagje : slagen)
			switch(slagje.dodeAanvallers)
			{
			case 0:
				if(slagje.dodeVerdedigers == 2)	kans2VerdedigerDood++;
				else							kans1VerdedigerDood++;
				break;

			case 1:
				if(slagje.dodeVerdedigers == 1)	kansEenOmEen++;
				else							kans1AanvalDood++;
				break;

			case 2:
				kans2AanvalDood++;
				break;

			default:	
				throw std::runtime_error("Hoeveel dode aanvallers zijn er dan??? " + std::to_string(slagje.dodeAanvallers));
			}

		kans2AanvalDood 	/= float(slagen.size());
		kans1AanvalDood 	/= float(slagen.size());
		kansEenOmEen 		/= float(slagen.size());
		kans1VerdedigerDood /= float(slagen.size());
		kans2VerdedigerDood /= float(slagen.size());

		std::cout 	<< std::fixed << std::setw(2)
					<< "Bij " << aanvalsDobbels << " aanvalsDobbels en " << verdedigingsDobbels << " verdedigingsDobbels zijn de kansen als volgt:\n"
					<< "Kans 2 aanvallers dood:  " << (kans2AanvalDood 		* 100) << "%\n"
					<< "Kans 1 aanvallers dood:  " << (kans1AanvalDood 		* 100) << "%\n"
					<< "Kans een om een:         " << (kansEenOmEen 		* 100) << "%\n"
					<< "Kans 1 verdediger dood:  " << (kans1VerdedigerDood 	* 100) << "%\n"
					<< "Kans 2 verdedigers dood: " << (kans2VerdedigerDood 	* 100) << "%\n" << std::endl;
	}

	int aanvalsDobbels, verdedigingsDobbels;

	float kans2AanvalDood=0, kans1AanvalDood=0, kansEenOmEen=0, kans1VerdedigerDood=0, kans2VerdedigerDood=0;
};

/*
struct spelers
{
	spelers(int anvallers, int verdedigers)
	: aanvallers(aanvallers), verdedigers(verdedigers)
	{}

	int aanvallers, verdedigers;
	
	bool operator<(const spelers & rechts) const 
	{
		return aanvallers < rechts.aanvallers || ( aanvallers == rechts.aanvallers && verdedigers < rechts.verdedigers);
	}

	float kansPerUitkomst(const spelers & uitkomst)
	{
		int aanvallersDood  = aanvallers  - uitkomst.aanvallers,
			verdedigersDood = verdedigers - uitkomst.verdedigers;

		assert(aanvallersDood  < 3);
		assert(verdedigersDood < 3);
		assert(aanvallersDood + verdedigersDood > 0);	


 	}


};
*/

int main(int argc, char *argv[])
{
	mogelijkeSlag(1, 1);
	mogelijkeSlag(2, 1);
	mogelijkeSlag(3, 1);

	mogelijkeSlag(1, 2);
	mogelijkeSlag(2, 2);
	mogelijkeSlag(3, 2);
}