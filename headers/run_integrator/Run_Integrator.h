/*
 * Run_Integrator.h
 *
 *  Created on: 20 Oct 2017
 *      Author: detlevcm
 */

#ifndef HEADERS_RUN_INTEGRATOR_H_
#define HEADERS_RUN_INTEGRATOR_H_

// Function to pick the right integration routine
void Choose_Integrator(
		Filenames,
		vector< double >,
		MechanismData ,
		//vector< string > ,vector< ThermodynamicData > ,	vector< SingleReactionData > &,
		InitParam,
		vector< double >&,
		PressureVesselCalc,
		vector< vector < str_RatesAnalysis > > &
);

// solve based on concentrations using Intel
void Integrate_Liquid_Phase_Intel(
		Filenames,
		vector< double >,
		MechanismData ,
		//vector< string > , vector< ThermodynamicData > , vector< SingleReactionData > &,
		InitParam,
		vector< double >&,
		vector< vector < str_RatesAnalysis > > &
);

// solve based on concentrations and consider headspace gas using Intel
void Integrate_Pressure_Vessel_Liquid_Phase_Intel(
		Filenames,
		vector< double >,
		MechanismData ,
		//vector< string > , vector< ThermodynamicData > , vector< SingleReactionData > &,
		InitParam,
		vector< double >&,
		PressureVesselCalc,
		vector< vector < str_RatesAnalysis > > &
);

// solve based on concentrations using Odepack LSODA
void Integrate_Liquid_Phase_Odepack_LSODA(
		Filenames,
		vector< double >,
		MechanismData ,
		//vector< string > ,vector< ThermodynamicData > ,vector< SingleReactionData > &,
		InitParam,
		vector< double >&,
		vector< vector < str_RatesAnalysis > > &
);

void Integrate_Gas_Phase_Odepack_LSODA(
		Filenames,
		vector< double >,
		MechanismData ,
		//vector< string > ,vector< ThermodynamicData > ,vector< SingleReactionData > &,
		InitParam,
		vector< double >&,
		vector< vector < str_RatesAnalysis > > &
);

// solve based on concentrations and consider headspace gas using Odepack LSODA
void Integrate_Pressure_Vessel_Liquid_Phase_Odepack_LSODA(
		Filenames,
		vector< double >,
		MechanismData ,
		//vector< string > ,vector< ThermodynamicData > ,vector< SingleReactionData > &,
		InitParam,
		vector< double >&,
		PressureVesselCalc,
		vector< vector < str_RatesAnalysis > > &
);

#endif /* HEADERS_RUN_INTEGRATOR_H_ */
