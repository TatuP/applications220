Info<< "Creating combustion model\n" << endl;

/*
autoPtr<combustionModels::psiChemistryCombustionModel> combustion
(
	combustionModels::psiChemistryCombustionModel::New
	(
		mesh
	)
);

autoPtr<combustionModels::psiChemistryCombustionModel> combustion
(
    combustionModels::psiChemistryCombustionModel::New
    (
        mesh
    )
);

psiChemistryModel& chemistry = combustion->pChemistry();

hsCombustionThermo& thermo = chemistry.thermo();

basicMultiComponentMixture& composition = thermo.composition();
PtrList<volScalarField>& Y = composition.Y();

*/
autoPtr<combustionModels::psiChemistryCombustionModel> combustion
(
    combustionModels::psiChemistryCombustionModel::New
    (
        mesh
    )
);

psiChemistryModel& chemistry = combustion->pChemistry();

hsCombustionThermo& thermo = chemistry.thermo();

basicMultiComponentMixture& composition = thermo.composition();
PtrList<volScalarField>& Y = composition.Y();
word inertSpecie(thermo.lookup("inertSpecie"));

volScalarField rho
(
    IOobject
    (
        "rho",
        runTime.timeName(),
        mesh,
	IOobject::READ_IF_PRESENT,
	IOobject::AUTO_WRITE
    ),
    thermo.rho()
);

Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);


volScalarField& p = thermo.p();
const volScalarField& psi = thermo.psi();
volScalarField& hs = thermo.hs();
const volScalarField& T = thermo.T();

#include "compressibleCreatePhi.H"

Info << "Creating turbulence model.\n" << nl;
autoPtr<compressible::turbulenceModel> turbulence
(
    compressible::turbulenceModel::New
    (
        rho,
        U,
        phi,
        thermo
    )
);

// Set the turbulence into the combustion model
combustion->setTurbulence(turbulence());


Info<< "Creating field dpdt\n" << endl;
volScalarField dpdt("dpdt", fvc::ddt(p));

Info<< "Creating field kinetic energy K\n" << endl;
volScalarField K("K", 0.5*magSqr(U));


multivariateSurfaceInterpolationScheme<scalar>::fieldTable fields;

forAll(Y, i)
{
    fields.add(Y[i]);
}
fields.add(hs);

volScalarField dQ
(
    IOobject
    (
        "dQ",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    mesh,
    dimensionedScalar("dQ", dimEnergy/dimTime, 0.0)
);


volScalarField kappa
(
    IOobject
    (
        "kappa",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    mesh,
    dimensionedScalar("kappa", dimless, 0.0)
);


volScalarField YinertNeg
(
    IOobject
    (
        "YinertNeg",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    mesh,
    dimensionedScalar("YinertNeg", dimEnergy/dimEnergy, 0.0)
);

surfaceScalarField phiHbyA // rUA stored for fixedFluxPressure BC
(
 	 IOobject
 	 (
 	 "(1|A(U_))",
	  runTime.timeName(),
	  mesh
	 ),
	 mesh,
	 //dimensionedScalar("rUA", dimTime, 1),
	 dimensionedScalar("phiHbyA",dimensionSet(1,0,-1,0,0),1)
	// zeroGradientFvPatchScalarField::typeName
);

