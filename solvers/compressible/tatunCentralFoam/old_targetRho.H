volScalarField sigmaDiffRho(rho*0.0);
	forAll(x,i) // loop through all cells
	{
		scalar xi = x[i]; // x position of cell i
		scalar AcellFace = 0.02e-2*(0.176022-0.1611)*1e-2; //mesh.magSf()[i]; // cell face area (in fuelAir channel)
		rhoUA[i].component(0) = rho[i]*U[i].component(0)*AcellFace;
		if (xi < xMax) // consider cells that are on the left side of xMax
		{
			scalar a = 10.0*(xi-xMin)/(xMax-xMin); // cell position scaled to 0 <= a <= 6.0
			scalar sigma = 0.5*(Foam::tanh(-a+7.0)+1.0);  // 0 < a < 6: sigma(a=0) ~ 1, sigma(a=6) ~ 0
			sigmaDiffRho[i] = sigma*(rho[i] - targetRho);
		}
	}
	solve
	(
		fvm::ddt(rho) + fvc::div(phi) == -sigmaDiffRho/tau
	);
