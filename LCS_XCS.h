/**
==================================

An implementation of the eXtended Classifier System (XCS). 

Tim Lukins (2002)

==================================
*/

// Inclusion guard:

#ifndef __XCS__
#define __XCS__

// Standard libraries used:

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <cmath>
#include <memory>

using namespace std;

////////////////////////////////////////////////////////////////
// Main class and interface definition:

namespace LCS {

	class XCS {

	public:

		// Accessible controlling parameters:

		double BETA;		// Learning rate.
		double GAMMA;		// Discount factor.
		double ALPHA;       // Adjustment in fitness calculation.
		long   ERROR;       // Value below which classifiers equal
		double VAL;			// Power parameter in fitness calculation.
		double EPSILON;     // Exploration probability.
		long   N;			// Maxsize of population (zero = no limit)
		double MU;			// Probability of mutation.
		double XU;			// Probability of crossover.
		double SIGMA;		// Mimimim fitness level.
		double PHASH;       // Probability of # when covering.
		long   THETAGA;     // GA application threshold
		long   THETADEL;    // GA culling threshold
		long   THETASUB;    // GA subsumption threshold
		long   THETAACT;    // Minimum actions in matchset before covering.

	public:

		// Definitions:

		typedef int Feature;	
		typedef vector<Feature> Perception;
		typedef long			Action;
		typedef vector<Action>	Actions;
		typedef long			Reward;

		// TODO: Exception class as well?

	public:

		// Constructor

		XCS(Actions);

		// Destructor

		virtual ~XCS();

		// Main methods

		void load(istream&);
		void save(ostream&); 
		void clear();
		//void step();
		Action act(Perception);
		void update(Reward);

		void learningOn(); 
		void learningOff();
		void subsumptionOn();
		void subsumptionOff();

		long populationSize();
		double internalPerformance();
		unsigned long currentTime();

	////////////////////////////////////////////////////////////////
	// Under the bonnet from here on...

	public:

		// Internal data structures:

		class Classifier {

		public:

			enum Symbol {ZERO=0,ONCE=1,DONT=2};

			XCS*			_system;
			vector<Symbol>  _condition; 
			Action			_action;
			double			_prediction;
			double			_error;
			double			_fitness;
			unsigned long	_experience;
			unsigned long	_timestamp;
			unsigned long	_actionsetsize;
			unsigned long	_numerosity;

		public:

			Classifier(XCS*);
			Classifier(const Classifier&);
			virtual ~Classifier();

			bool matches(Perception);
			void cover(Perception,Action);
			//Classifier* copy();
			void assign(vector<Symbol>,Action,double,double,double,unsigned long,unsigned long, unsigned long, unsigned long);

			friend class XCS;
		};

	private:

		// Control switches:

		bool doSubsumption;
		bool doLearning;

		// Data:

		typedef vector<Classifier*> ClassifierList;
		typedef vector<Classifier*>::iterator ClassifierIter;

		typedef vector<Action>::iterator ActionIter;

		ClassifierList	_population;
		ClassifierList	_matchset;
		ClassifierList	_actionset;
		Reward			_reward;
		Perception		_percept;
		Actions			_actions;
		Action			_proposed;

		unsigned long   _time;
		long			_seed;
		double			_reinforced;

		friend class Classifier; // Allow classifier to access its system...

	private:

		// Internal algorithm methods:

		void generateMatchset();
		void selectAction();
		void generateActionSet();
		void updatePrediction();
		void updateFitness();
		void applyGA();
		XCS::Classifier* selectOffspring();
		void applyCrossover(Classifier*,Classifier*);
		void applyMutation(Classifier*);
		void insertIntoPopulation(Classifier*);
		void deleteFromPopulation();
		double deletionVote(Classifier*,double);
		void doActionSetSubsumption();
		bool couldSubsume(Classifier*);
		long countGenerality(Classifier*);
		bool moreGeneral(Classifier*,Classifier*);
		bool doesSubsume(Classifier*,Classifier*);

		// Utility methods:

		double drand();

#ifdef TEST
	public:
					void test();
#endif
	};

} // End namespace LCS


#endif
