within;
package OpenISL "OpenISL library"
  import Modelica.Blocks.Interfaces;
  import Modelica.Utilities.System;
  annotation (
    preferedView="info",
    version="1.0",
    versionBuild=2,
    versionDate="2022-11-11",
    revisionId="$Id:: package.mo 00001 2022-11-11 10:00:00Z#$",
    Documentation(info="<html>
<p>ISL library</p>
Copyright &copy; 2019-2023, E-Sim Solutions Inc
</p>
</html>"));

  model ISLParameter "ISL Parameters"
    parameter String XMLFile=""
      "XML file (model name/file name without path)";
    parameter String ISLSession=""
      "Session Id";

    annotation (
    defaultComponentName="islparameters",
    defaultComponentPrefixes="inner",
    missingInnerMessage="
Your model is using an outer \"islparameters\" component but
an inner \"islparameters\" component is not defined.
For simulation drag OpenISL.ISLParameter into your model.",
      Documentation(info="
<HTML>
<p>
ISL Parameters.
</p>
</HTML>
"),
      Icon(coordinateSystem(
          preserveAspectRatio=true,
          extent={{-100,-100},{100,100}},
          grid={2,2}), graphics={
          Polygon(
            points={{-100,-80},{-100,80},{100,80},{100,-80},{-100,-80}},
            lineColor={0,0,127},
            fillColor={112,190,255},
            fillPattern=FillPattern.Solid),
          Text(
            extent={{-150,140},{150,100}},
            textString="ISL Parameters",
            lineColor={0,0,255}),
          Text(
            extent={{-100,40},{100,-40}},
            textString="ISL\nSettings",
            lineColor={0,0,0})}),
          Diagram(coordinateSystem(
          preserveAspectRatio=true,
          extent={{-100,-100},{100,100}},
          grid={2,2}), graphics={Polygon(
            points={{-100,-80},{-100,80},{100,80},{100,-80},{-100,-80}},
            lineColor={0,0,127},
            fillColor={112,190,255},
            fillPattern=FillPattern.Solid)}));

  end ISLParameter;

  block ISLRealIn "ISL Real Input"

    parameter String VarName="in"
      "Variable name";
	parameter Boolean IsFixedStep=true
	  "Fixed step parameter";
    outer OpenISL.ISLParameter islparameters "ISL Parameters";

    public
      Interfaces.RealOutput y "Output signal connector"
        annotation (Placement(transformation(extent={{100,-10},{120,10}},
          rotation=0)));

    protected
      parameter Integer m_nId(fixed=false)
        "Variable index";
      parameter Real m_dStepSize(fixed=false)
        "Step size";
	  output Boolean bSampleTrigger
		"Sample event";

      function ISLMInit
        input Integer nISLId;
        input String sXMLFile;
        input String sSession;
        input String sName;
        output Integer nId;
      external "C" nId = ISLInitialize(nISLId, sXMLFile, sSession, sName)
        annotation(Library="isl_modelica",
                   Include="#include \"isl_modelica.h\"",
				   IncludeDirectory="modelica://OpenISL/Resources/Include",
				   LibraryDirectory="modelica://OpenISL/Resources/Library");
      end ISLMInit;

      function ISLMGetStep
        input Integer nId;
        output Real dStepSize;
      external "C" dStepSize = ISLGetStep(nId)
        annotation(Library="isl_modelica",
                   Include="#include \"isl_modelica.h\"",
				   IncludeDirectory="modelica://OpenISL/Resources/Include",
				   LibraryDirectory="modelica://OpenISL/Resources/Library");
      end ISLMGetStep;

      function ISLMGetReal
        input Integer nId;
        input Real dTime;
        input Boolean bEnd;
        output Real dValOut;
      external "C" dValOut = ISLCGetReal(nId, dTime, bEnd)
        annotation(Library="isl_modelica",
                   Include="#include \"isl_modelica.h\"",
				   IncludeDirectory="modelica://OpenISL/Resources/Include",
				   LibraryDirectory="modelica://OpenISL/Resources/Library");
      end ISLMGetReal;

    initial algorithm
      m_nId := ISLMInit(358,
		Modelica.Utilities.Files.loadResource("modelica://" + islparameters.XMLFile),
		islparameters.ISLSession, VarName);
	  m_dStepSize := ISLMGetStep(m_nId);
    equation
	  if IsFixedStep then
	    bSampleTrigger = sample(0.0, m_dStepSize);
	    when bSampleTrigger then
	      y = ISLMGetReal(m_nId, time, terminal());
	    end when;
	  else
        y = ISLMGetReal(m_nId, time, terminal());
	  end if;

    annotation (
      Documentation(info="
<HTML>
<p>
ISL real input.
</p>
</HTML>
"),
      Icon(coordinateSystem(
          preserveAspectRatio=true,
          extent={{-100,-100},{100,100}},
          grid={2,2}), graphics={
          Polygon(
            points={{-100,-80},{-100,80},{100,80},{100,-80},{-100,-80}},
            lineColor={0,0,127},
            fillColor={112,190,255},
            fillPattern=FillPattern.Solid),
          Text(
            extent={{-150,140},{150,100}},
            textString="%VarName",
            lineColor={0,0,255}),
          Text(
            extent={{-100,40},{100,-40}},
            textString="ISL\nInput",
            lineColor={0,0,0})}),
          Diagram(coordinateSystem(
          preserveAspectRatio=true,
          extent={{-100,-100},{100,100}},
          grid={2,2}), graphics={Polygon(
            points={{-100,-80},{-100,80},{100,80},{100,-80},{-100,-80}},
            lineColor={0,0,127},
            fillColor={112,190,255},
            fillPattern=FillPattern.Solid)}));

  end ISLRealIn;

  block ISLRealOut "ISL Real Output"

    parameter String VarName="out"
      "Variable name";
	parameter Boolean IsFixedStep=true
	  "Fixed step parameter";
    outer OpenISL.ISLParameter islparameters "ISL Parameters";

    public
      Interfaces.RealInput x "Input signal connector"
        annotation (Placement(transformation(extent={{-140,-20},{-100,20}},
          rotation=0)));

    protected
      parameter Integer m_nId(fixed=false)
        "Variable index";
      parameter Real m_dStepSize(fixed=false)
        "Step size";
	  output Boolean bSampleTrigger
		"Sample event";
      Integer m_nRet;

      function ISLMInit
        input Integer nISLId;
        input String sXMLFile;
        input String sSession;
        input String sName;
        output Integer nId;
      external "C" nId = ISLInitialize(nISLId, sXMLFile, sSession, sName)
        annotation(Library="isl_modelica",
                   Include="#include \"isl_modelica.h\"",
				   IncludeDirectory="modelica://OpenISL/Resources/Include",
				   LibraryDirectory="modelica://OpenISL/Resources/Library");
      end ISLMInit;

      function ISLMGetStep
        input Integer nId;
        output Real dStepSize;
      external "C" dStepSize = ISLGetStep(nId)
        annotation(Library="isl_modelica",
                   Include="#include \"isl_modelica.h\"",
				   IncludeDirectory="modelica://OpenISL/Resources/Include",
				   LibraryDirectory="modelica://OpenISL/Resources/Library");
      end ISLMGetStep;

      function ISLMSetReal
        input Integer nId;
        input Real dVal;
        input Real dTime;
        input Boolean bEnd;
        output Integer nRet;
      external "C" nRet = ISLCSetReal(nId, dVal, dTime, bEnd)
        annotation(Library="isl_modelica",
                   Include="#include \"isl_modelica.h\"",
				   IncludeDirectory="modelica://OpenISL/Resources/Include",
				   LibraryDirectory="modelica://OpenISL/Resources/Library");
      end ISLMSetReal;

    initial algorithm
      m_nId := ISLMInit(358,
		Modelica.Utilities.Files.loadResource("modelica://" + islparameters.XMLFile),
		islparameters.ISLSession, VarName);
	  m_dStepSize := ISLMGetStep(m_nId);
    equation
	  if IsFixedStep then
	    bSampleTrigger = sample(0.0, m_dStepSize);
	    when bSampleTrigger then
          m_nRet = ISLMSetReal(m_nId, x, time, terminal());
	    end when;
	  else
        m_nRet = ISLMSetReal(m_nId, x, time, terminal());
	  end if;

    annotation (
      Documentation(info="
<HTML>
<p>
ISL real output.
</p>
</HTML>
"),
      Icon(coordinateSystem(
          preserveAspectRatio=true,
          extent={{-100,-100},{100,100}},
          grid={2,2}), graphics={
          Polygon(
            points={{-100,-80},{-100,80},{100,80},{100,-80},{-100,-80}},
            lineColor={0,0,127},
            fillColor={112,190,255},
            fillPattern=FillPattern.Solid),
          Text(
            extent={{-150,140},{150,100}},
            textString="%VarName",
            lineColor={0,0,255}),
          Text(
            extent={{-100,40},{100,-40}},
            textString="ISL\nOutput",
            lineColor={0,0,0})}),
          Diagram(coordinateSystem(
          preserveAspectRatio=true,
          extent={{-100,-100},{100,100}},
          grid={2,2}), graphics={Polygon(
            points={{-100,-80},{-100,80},{100,80},{100,-80},{-100,-80}},
            lineColor={0,0,127},
            fillColor={112,190,255},
            fillPattern=FillPattern.Solid)}));

  end ISLRealOut;

end OpenISL;

