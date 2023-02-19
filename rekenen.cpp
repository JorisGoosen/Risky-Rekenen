
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <vector>
#include <exception>
#include <iomanip>
#include <sstream>

//clear && gcc -o reken rekenen.cpp --std=c++20

enum class strategie { altijd2Verdedigen, altijd1Verdedigen, besteKansVerdedigen };

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

	worpAanval(const worpAanval & ander)
	{
		for(size_t i=0;i<3;i++)
			stenen[i] = ander[i];

		aantalStenen = ander.aantalStenen;
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

	worpVerdediging(const worpVerdediging & ander)
	{
		for(size_t i=0;i<2;i++)
			stenen[i] = ander[i];

		aantalStenen = ander.aantalStenen;
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
		dodeAanvallers 	= berekenDodeAanvallers();
		dodeVerdedigers = berekenDodeVerdedigers();
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
			return (aanval[0] <= verdediging[0]); //want gesorteerd
			
		case 2:
		case 3:
			return (aanval[0] <= verdediging[0]) + ((aanval[1] <= verdediging[1]) && verdediging.aantalStenen == 2);
		}

		std::cerr << "aanval.aantalStenen is: " << aanval.aantalStenen << std::endl;
		throw std::runtime_error("berekenDodeAanvallers???");
	}

	int berekenDodeVerdedigers()
	{
		switch(aanval.aantalStenen)
		{
		case 1:
			return (aanval[0] > verdediging[0]); //want gesorteerd dus de hoogste verdediging staat al vooran
			
		case 2:
		case 3:
			return (aanval[0] > verdediging[0]) + (aanval[1] > verdediging[1] && verdediging.aantalStenen == 2);
		}

		std::cerr << "verdediging.aantalStenen is: " << verdediging.aantalStenen << std::endl;
		throw std::runtime_error("berekenDodeVerdedigers???");
	}

	worpAanval		aanval;
	worpVerdediging verdediging;
	int 			dodeAanvallers, dodeVerdedigers;

};

struct mogelijkeSlag
{
	mogelijkeSlag(){}

	mogelijkeSlag(int aanvalsDobbels, int verdedigingsDobbels)
	: aanvalsDobbels(aanvalsDobbels), verdedigingsDobbels(verdedigingsDobbels)
	{
		std::vector<worpAanval> 		aanvallen;
		std::vector<worpVerdediging>	verdedigingen;
		std::vector<slag>				slagen;

		//loop door alle mogelijke sets aanvaldobbelstenen en gooi ze in een vector
		for(size_t a1=1; a1<7; a1++)
			for(size_t a2=aanvalsDobbels < 2 ? 0 : 1; a2< (aanvalsDobbels < 2 ? 1 : 7); a2++)
				for(size_t a3=aanvalsDobbels < 3 ? 0 : 1; a3< (aanvalsDobbels < 3 ? 1 : 7); a3++)
					aanvallen.push_back(worpAanval(a1, a2, a3));

		//idem voor verdedigers
		for(size_t v1=1; v1<7; v1++)
			for(size_t v2=verdedigingsDobbels < 2 ? 0 : 1; v2< (verdedigingsDobbels < 2 ? 1 : 7); v2++)
					verdedigingen.push_back(worpVerdediging(v1, v2));

		//vermenigvuldigen we die twee sets met elkaar
		for(worpAanval & aanval : aanvallen)
			for(worpVerdediging & verdediging : verdedigingen)
				slagen.push_back(slag(aanval, verdediging));

		//en tellen maar
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

		kans2AanvalDood 	/= slagen.size();
		kans1AanvalDood 	/= slagen.size();
		kansEenOmEen 		/= slagen.size();
		kans1VerdedigerDood /= slagen.size();
		kans2VerdedigerDood /= slagen.size();

		printJezelf();

	}

	int aanvalsDobbels, verdedigingsDobbels;

	float kans2AanvalDood=0, kans1AanvalDood=0, kansEenOmEen=0, kans1VerdedigerDood=0, kans2VerdedigerDood=0;

	///Is dit een goede strategie?
	float verdedigingsFitness() const
	{
		return 	 3 * kans2AanvalDood 	 +
				 2 * kans1AanvalDood 	 +
					 kansEenOmEen		 +
				-2 * kans1VerdedigerDood +
				-3 * kans2VerdedigerDood ;
	}

	void printJezelf() const 
	{

		std::cout 	<< std::fixed << std::setw(2)
					<< "Bij " << aanvalsDobbels << " aanvalsDobbels en " << verdedigingsDobbels << " verdedigingsDobbels zijn de kansen als volgt:\n"
					<< "Kans 2 aanvallers dood:  " << (kans2AanvalDood 		* 100) << "%\n"
					<< "Kans 1 aanvallers dood:  " << (kans1AanvalDood 		* 100) << "%\n"
					<< "Kans een om een:         " << (kansEenOmEen 		* 100) << "%\n"
					<< "Kans 1 verdediger dood:  " << (kans1VerdedigerDood 	* 100) << "%\n"
					<< "Kans 2 verdedigers dood: " << (kans2VerdedigerDood 	* 100) << "%\n"
					<< "Samengeteld: " << (kans2AanvalDood + kans1AanvalDood + kansEenOmEen + kans1VerdedigerDood + kans2VerdedigerDood) * 100 << "%\n" << std::endl;
	}
};

struct mogelijkeSlagen
{
	mogelijkeSlagen()
	{
		for(size_t a=1; a<4; a++)
			for(size_t v=1; v<3; v++)
			{
				mogelijkeSlag * s = new mogelijkeSlag(a, v);

				mogelijkheden.insert(s);
				mogelijkPerVerdedigersPerAanvallers[a][v] = s;
			}	
	}

	const mogelijkeSlag * slagVoorStrategie(int aanvallersOver, int verdedigersOver, strategie strat = strategie::altijd2Verdedigen)
	{
		int aanval 		= std::min(aanvallersOver, 3),
			verdediging;
		
		switch(strat)
		{
		case strategie::altijd2Verdedigen:
			verdediging = 2;
			break;

		case strategie::altijd1Verdedigen:
			verdediging = 1;
			break;

		case strategie::besteKansVerdedigen:
			if(verdedigersOver > 1)
			{
				//hier moet dus eigenlijk voor iedere mogelijke worp vn de aanvaller worden gekeken welke mogelijkheid van 1 of 2 verdedigers een goed idee is.
				//maar wat is een goed idee? Zo min mogelijk verliezen? Of vermoorden of wat?
				const mogelijkeSlag * een  = mogelijkPerVerdedigersPerAanvallers[aanval][1],
									* twee = mogelijkPerVerdedigersPerAanvallers[aanval][2];
				
				verdediging =  een->verdedigingsFitness() > twee->verdedigingsFitness() ? 1 : 2;

				/*static std::map<int, std::set<int>> alGezegd;

				if(alGezegd.count(aanvallers) == 0 || alGezegd[aanvallers].count(
				std::cout << "Bij de keuze voor 1 of 2 verdedigers (uit " << verdedigersOver << ") is gekozen voor " << verdediging << " vanwege volgende slagen:\n";
				een->printJezelf();
				twee->printJezelf();*/
			}
			else
				verdediging = 1;

			break;
		}

		return mogelijkPerVerdedigersPerAanvallers[std::min(aanvallersOver, aanval)][std::min(verdedigersOver, verdediging)];
	}

	std::set<mogelijkeSlag*> 						mogelijkheden;
	std::map<int, std::map<int, mogelijkeSlag*>>	mogelijkPerVerdedigersPerAanvallers; ///< [aanvallers][verdedigers]
};


struct oorlog
{
	oorlog(int aanvallers, int verdedigers, strategie strat = strategie::altijd2Verdedigen)
	: aanvallers(aanvallers), verdedigers(verdedigers), strat(strat)
	{
		assert(aanvallers != 0 || verdedigers != 0);

		oorlogen[strat][aanvallers][verdedigers] = this;

		if(aanvallers == 0 || verdedigers == 0)
		{
			kansAanvallerWint = verdedigers == 0;
			eindSlag = true;

			return; //Einde!
		}

		dood1Aanval 	= vindOorlog(aanvallers-1, verdedigers);
		dood1Verdediger = vindOorlog(aanvallers,   verdedigers-1);

		deSlag = mogelijkheden.slagVoorStrategie(aanvallers, verdedigers, strat);

		if(aanvallers >= 2)
			dood2Aanval = vindOorlog(aanvallers - 2, verdedigers);

		if(verdedigers >= 2)
			dood2Verdediger = vindOorlog(aanvallers, verdedigers - 2);

		if(aanvallers >= 1 && verdedigers >= 1 && (aanvallers > 1 || verdedigers > 1))
			eenOmEen = vindOorlog(aanvallers - 1, verdedigers - 1);
	}

	oorlog * vindOorlog(int aanvallers, int verdedigers)
	{
		if(	oorlogen.count(strat)	&& 
			oorlogen[strat].count(aanvallers)	&&
			oorlogen[strat][aanvallers].count(verdedigers)	
		)
			return oorlogen[strat][aanvallers][verdedigers];
		
		return new oorlog(aanvallers, verdedigers, strat);
	}

	
	bool operator<(const oorlog & rechts) const 
	{
		return aanvallers < rechts.aanvallers || ( aanvallers == rechts.aanvallers && verdedigers < rechts.verdedigers) || (aanvallers == rechts.aanvallers && verdedigers == rechts.verdedigers && int(strat) < int(rechts.strat));
	}

	bool operator==(const oorlog & rechts) const 
	{
		return aanvallers == rechts.aanvallers && verdedigers == rechts.verdedigers && strat == rechts.strat;
	}

	float watIsDeKansDatAanvallerWint()
	{
		if(eindSlag)
		{
			assert(kansAanvallerWint > -1);
			return kansAanvallerWint;
		}

		if(!deSlag)
			std::cerr << "\n\nOorlog " << str() << " heeft geen deSlag" << std::endl;

		if(kansAanvallerWint == -1)
			kansAanvallerWint =
				(!dood2Aanval 		? 0 :     dood2Aanval->watIsDeKansDatAanvallerWint() * deSlag->kans2AanvalDood		) + 
				(!dood1Aanval 		? 0 :     dood1Aanval->watIsDeKansDatAanvallerWint() * deSlag->kans1AanvalDood		) + 
				(!eenOmEen 			? 0 :        eenOmEen->watIsDeKansDatAanvallerWint() * deSlag->kansEenOmEen			) + 
				(!dood1Verdediger 	? 0 : dood1Verdediger->watIsDeKansDatAanvallerWint() * deSlag->kans1VerdedigerDood	) + 
				(!dood2Verdediger 	? 0 : dood2Verdediger->watIsDeKansDatAanvallerWint() * deSlag->kans2VerdedigerDood	) ;
		
		return kansAanvallerWint;
	}

	static std::map<strategie, std::map<int, std::map<int, oorlog *>>> 	oorlogen;
	static mogelijkeSlagen												mogelijkheden;

	oorlog				* 	dood2Aanval				= nullptr, 
						* 	dood1Aanval				= nullptr, 
						* 	eenOmEen				= nullptr, 
						* 	dood1Verdediger			= nullptr, 
						* 	dood2Verdediger			= nullptr;
	int 					aanvallers, 
							verdedigers;
	bool					eindSlag 				= false;
	strategie				strat;
	mogelijkeSlag const	*	deSlag					= nullptr;
	float 					kansAanvallerWint 		= -1;

	std::string str()
	{
		return "veldslag A: " + std::to_string(aanvallers) + " V: " + std::to_string(verdedigers);
	}

	std::string uitslag()
	{
		std::stringstream uit;

		uit << "Een veldslag met " << str() << " heeft een kans van " << std::fixed  << std::setw(3) << std::setprecision(2) << std::setfill(' ') << watIsDeKansDatAanvallerWint() * 100 << "% bij strategie " << (strat == strategie::altijd1Verdedigen ? "altijd 1 verdedigen" : strat == strategie::altijd2Verdedigen ? "altijd 2 verdedigen" : "\"beste kans verdedigen\"");

		return uit.str();
	}
};

std::map<strategie, std::map<int, std::map<int, oorlog *>>> 	oorlog::oorlogen = std::map<strategie, std::map<int, std::map<int, oorlog *>>> ();
mogelijkeSlagen oorlog::mogelijkheden = mogelijkeSlagen();

int main(int argc, char *argv[])
{

	for(size_t aanvallers=1; aanvallers<30; aanvallers+= aanvallers < 5 ? 1 : 3)
		for(size_t verdedigers=1; verdedigers<30; verdedigers+= verdedigers < 5 ? 1 : 3)
		{
			oorlog 	*	veldslag2 = new oorlog(aanvallers, verdedigers, strategie::altijd2Verdedigen), 
					*	veldslag1 = new oorlog(aanvallers, verdedigers, strategie::altijd1Verdedigen),
					*	veldslagB;// beste kans strategie is niet goed geimplementeerd = new oorlog(aanvallers, verdedigers, strategie::besteKansVerdedigen);

			std::cout << "\n\n" << veldslag1->uitslag() << "\n" << veldslag2->uitslag();// << "\n" << veldslagB->uitslag();
		}
}