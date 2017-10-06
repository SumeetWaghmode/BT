<?xml version="1.0" encoding="UTF-8"?>
<!--
	title: MD-Report: Multi-level parts list
	desc.: xml 2 pdf

	time: 24.05.2011
-->
<!DOCTYPE stylesheet [
	<!ENTITY eol '<xsl:text>
</xsl:text>'>
	<!ENTITY sep '<xsl:text>";"</xsl:text>'>
]>
<xsl:stylesheet version="1.0" 
	xmlns:fo="http://www.w3.org/1999/XSL/Format" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:sbtrpt="http://mrtplm.com/pdm/xmlrep"
	xmlns:rptsup="http://mrtplm.com/pdm/xmlrep-support"
	xmlns:xalan="http://xml.apache.org/xalan"
	xmlns:str="http://exslt.org/strings"
	xmlns:exslt="http://exslt.org/common"
	xmlns:js="http://www.example.com/results"		extension-element-prefixes="js">
	
	<xsl:output indent="yes" method="xml" encoding="UTF-8"/>

	<xalan:component prefix="js" elements="init add_result set_specialline add_sortnr  init_sortarray" functions="init add_result set_specialline add_sortnr  init_sortarray j_test generate_output get_specialline() get_sortnr_str()">
		<xalan:script lang="javascript">
			var levelarray;
			var specialline = 0;
			
			var sortarray;
			
			
			function get_sortnr_str()
			{
				sortarray.sort();
				var returnstring="";
				for (var i = 0; sortarray.length > i; i++)
				{
					var entry = sortarray[i];
					returnstring.concat(entry);
					returnstring.concat(";");
				}
			
				//sortarray = new Array(1000);
				
			return returnstring;
			}
			
			function add_sortnr(docnr)
			{
				sortarray.push(docnr);
			
			return null;
			}
			
			function init_sortarray()
			{
				sortarray = new Array(1000);
				return null;
			}
			
			function init()
			{
				levelarray = new Array(100000);
				for (var i = 0; levelarray.length > i; i++)
				{
					levelarray[i] = new Array("");
				}
			return null;
			}
			
			function j_test()
			{
			return "test";
			}
			
			function set_specialline(input)
			{
			specialline=input;
			return null;
			}
			
			function get_specialline()
			{
			return specialline;
			}
			
			function add_result(level, linetext)
			{
				if(linetext.charAt(0)!= ";")
				{
					if(parseInt(level) > 1)
					{
							var numPointStr = "";
							
							for(var i = 1; i &lt; parseInt(level); i++)
							{
								numPointStr = numPointStr + ".";
							}
							
							levelarray[0].push(numPointStr.concat(linetext));
							return null;
					}
					else
					{
						levelarray[0].push(linetext);
						return null;
					}
				}
				else
				{
					levelarray[0].push(linetext);
					return null;
				}
			}
			
			
			function generate_output()
			{
				var str_output="";
				//str_output = str_output + "test";	
			
			
			for (var i = 0; i &lt; levelarray.length; i++)
				{
					//str_output = str_output + " Gruppe" + i +": " ;	
					for (var y = 0;y &lt; levelarray[i].length; y++)
					{
						if(levelarray[i][y] != "")
						{
							str_output = str_output + levelarray[i][y];
							str_output = str_output + "§$";
					}	}
				}
			return str_output;
			}
			
		</xalan:script>
	</xalan:component>

	
	
	<xsl:param name="l0ReportLocale"/>
	<xsl:param name="m0LastRevComplete"/>
	<xsl:param name="m0ProcessingLvl"/>
	<xsl:param name="m0InclPCBStruc"/>
	
	<!--xsl:param name="l0ReportLocale"/>
	<xsl:param name="m0LastRevComplete"/>
	<xsl:param name="m0ProcessingLvl"/>
	<xsl:param name="m0InclPCBStruc"/-->
	
	<xsl:param name="lvlinput"/>
	
	<xsl:variable name="doc_str" select="'dclass_dev,dclass_manu,dclass_UaD,dclass_MaP'"/>
	
	<xsl:variable name="doc_rel_str" select="'J0DocStr,l0DocPrt,PartDoc,l0PrDcLv'"/>
	<xsl:variable name="part_rel_str" select="'l0AuRvRv,AsRevRev'"/>
	
	<xsl:variable name="reportDate">		
		<xsl:value-of select="sbtrpt:ResultSet/@Date"/>
	</xsl:variable>
	
	<xsl:variable name="reportTime">
		<xsl:value-of select="sbtrpt:ResultSet/@Time"/>
	</xsl:variable>
	
	<xsl:variable name="reportTitleHeading">
		<xsl:choose>
			<xsl:when test="not($m0ProcessingLvl = '1')">				<xsl:choose>					<xsl:when test="$l0ReportLocale = 'DE'">
						<xsl:value-of select="'Mehrebenen-Teileliste (mit Dokumenten)'"/>
					</xsl:when>
					<xsl:when test="$l0ReportLocale = 'EN'">
						<xsl:value-of select="'Multi-level parts list (with documents)'"/>
					</xsl:when>								</xsl:choose>							</xsl:when>						<xsl:when test="$m0ProcessingLvl = '1'">							<xsl:choose>									<xsl:when test="$l0ReportLocale = 'DE'">											<xsl:value-of select="'Einebenen-Teileliste (mit Dokumenten)'"/>											</xsl:when>										<xsl:when test="$l0ReportLocale = 'EN'">											<xsl:value-of select="'Single-level parts list (with documents)'"/>											</xsl:when>									</xsl:choose>									</xsl:when>
		</xsl:choose>
	</xsl:variable>
	
	<xsl:variable name="PCBYesNo">
		
		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'EN'">
				<xsl:choose>
					<xsl:when test="$m0InclPCBStruc = 'true'">
						<xsl:value-of select="'Yes'"/>
					</xsl:when>
					
					<xsl:when test="$m0InclPCBStruc = 'false'">
						<xsl:value-of select="'No'"/>
					</xsl:when>
					
				</xsl:choose>
			</xsl:when>
		</xsl:choose>
		
		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'DE'">
				<xsl:choose>
					<xsl:when test="$m0InclPCBStruc = 'true'">
						<xsl:value-of select="'Ja'"/>
					</xsl:when>
					<xsl:when test="$m0InclPCBStruc = 'false'">
						<xsl:value-of select="'Nein'"/>
					</xsl:when>
				</xsl:choose>
			</xsl:when>
		</xsl:choose>
	</xsl:variable>
	

	<xsl:variable name="ConfigContext">
		<xsl:value-of select="$m0LastRevComplete"/>
		<!--xsl:choose>
			<xsl:when test="$l0ReportLocale = 'DE'">
				<xsl:choose>
					<xsl:when test="$m0LastRevComplete = '+'">
						<xsl:value-of select="'Letzte Revision und Definitiv'"/>
					</xsl:when>
					
					<xsl:when test="$m0LastRevComplete = '-'">
						<xsl:value-of select="'Letzte Revision'"/>
					</xsl:when>
					
				</xsl:choose>
			</xsl:when>
		</xsl:choose>
		
		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'EN'">
				<xsl:choose>
					<xsl:when test="$m0LastRevComplete = '+'">
						<xsl:value-of select="'Last Revision and Complete'"/>
					</xsl:when>
					<xsl:when test="$m0LastRevComplete = '-'">
						<xsl:value-of select="'Last Revision'"/>
					</xsl:when>
				</xsl:choose>
			</xsl:when>
		</xsl:choose-->
	</xsl:variable>
	
	
	<!-- fix for line-breakes without spaces (techinfo) -->
	<xsl:template name="zero_width_space_1">
		<xsl:param name="data"/>
		<xsl:param name="counter" select="0"/>
		
		<xsl:if test="$counter &#60;&#61; string-length($data)">
			<xsl:value-of select='concat(substring($data, $counter, 1),"​")'/>
			<xsl:call-template name="zero_width_space_2">
				<xsl:with-param name="data" select="$data"/>
				<xsl:with-param name="counter" select="$counter+1"/>
			</xsl:call-template>
		</xsl:if>
	</xsl:template>

	<xsl:template name="zero_width_space_2">
		<xsl:param name="data"/>
		<xsl:param name="counter"/>
		
		<xsl:value-of select='concat(substring($data, $counter, 1),"​")'/>
		<xsl:call-template name="zero_width_space_1">
			<xsl:with-param name="data" select="$data"/>
			<xsl:with-param name="counter" select="$counter+1"/>
		</xsl:call-template>
	</xsl:template> 
	<!-- END: fix for line-breakes without spaces (techinfo) -->
	
		
	<xsl:template match="sbtrpt:ResultSet">	
		<js:init/>

		<fo:root xmlns:fo="http://www.w3.org/1999/XSL/Format" line-height="1">
			<fo:layout-master-set>
				<fo:simple-page-master master-name="A4-landscape" page-height="21cm" page-width="29.7cm" margin-top="1cm" margin-bottom="1cm" margin-left="1cm" margin-right="1cm">
					<fo:region-body margin-top="4.7cm"/>
					<fo:region-before extent="2cm"/>
					<fo:region-after margin-bottom="1cm"/>
				</fo:simple-page-master>
			</fo:layout-master-set>
			
			<xsl:apply-templates />
		</fo:root>
	</xsl:template>
	
	<xsl:template match="sbtrpt:Node">
		<fo:page-sequence master-reference="A4-landscape" force-page-count="no-force" initial-page-number="1">
			<fo:static-content flow-name="xsl-region-before">
				<xsl:call-template name="header"/>
			</fo:static-content>
			
			<fo:static-content flow-name="xsl-region-after">
				<xsl:call-template name="footer"/>
			</fo:static-content>
			
			<fo:flow flow-name="xsl-region-body">
				<xsl:call-template name="body"/>
				<xsl:call-template name="output"/>
				<fo:block id="end"/>
			</fo:flow>
			
		</fo:page-sequence>
	</xsl:template>
	
	<xsl:template name="output">
		<xsl:param name="isseperatorline"/>
		
		<fo:table table-layout="fixed" width="27.7cm" overflow="hidden">
			<fo:table-column column-width="1cm"/>
			<fo:table-column column-width="1cm"/>
			<fo:table-column column-width="1cm"/>
			<fo:table-column column-width="1cm"/>
			<fo:table-column column-width="2.54cm"/>
			<fo:table-column column-width="1cm"/>
			<fo:table-column column-width="2.5cm"/>
			<fo:table-column column-width="4.415cm"/>
			<fo:table-column column-width="4.415cm"/>
			<fo:table-column column-width="4.415cm"/>
			<fo:table-column column-width="4.415cm"/>

			<fo:table-body font-size="8pt">
				<xsl:for-each select="str:split(js:generate_output(), '§$')">
					<fo:table-row>
						<xsl:if test="(position()=1)">
							<xsl:value-of select="js:set_specialline(2)"/>
						</xsl:if>
						
						<xsl:variable name="tmpline" select ="./."/>
						<xsl:for-each select="str:split($tmpline, '&#254;')">
							<xsl:if test="((string-length(./node()) &gt; 0) and not(js:get_specialline()=2))">
								<xsl:value-of select="js:set_specialline(1)"/>
							</xsl:if>

							<xsl:choose>
								
								<xsl:when test="(js:get_specialline()=1)">
									<fo:table-cell display-align="auto" padding-left="1pt" padding-top="1pt" border-top="1pt solid black">
										<fo:block-container overflow="hidden">
											<fo:block>
													<xsl:value-of select="./."/>
											</fo:block>
										</fo:block-container>
									</fo:table-cell>
								</xsl:when>
								<xsl:otherwise>
									<xsl:if test="((position()&lt;5) or (js:get_specialline()=2))">
										<fo:table-cell display-align="auto" padding-left="1pt" padding-top="1pt">
											<fo:block-container overflow="hidden">
												<fo:block>
													<xsl:value-of select="./."/>
												</fo:block>
											</fo:block-container>
										</fo:table-cell>
									</xsl:if>
									<xsl:if test="(position()&gt;4 and not(js:get_specialline()=2))">
										<fo:table-cell display-align="auto" padding-left="1pt" padding-top="1pt" >
											<fo:block-container overflow="hidden">
												<fo:block>
													<xsl:value-of select="./."/>
												</fo:block>
											</fo:block-container>
										</fo:table-cell>
									</xsl:if>
								</xsl:otherwise>
							</xsl:choose>	
						</xsl:for-each>
					</fo:table-row>
					<xsl:value-of select="js:set_specialline(0)"/>
				</xsl:for-each>	
			</fo:table-body>	
		</fo:table>
	</xsl:template>

	<xsl:template name="header">
		
		<fo:table table-layout="fixed" width="27.7cm">
				<fo:table-column column-width="3.4625cm"/>
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="2.4625cm"/>
				<fo:table-column column-width="3.4625cm"/>
				<fo:table-column column-width="5.4625cm"/>
				<fo:table-column column-width="3.4625cm"/>
				<fo:table-column column-width="4.925cm"/>
				<fo:table-column column-width="3.4625cm"/>
			<xsl:choose>
				<xsl:when test="$l0ReportLocale = 'DE'">
					<fo:table-body font-size="8pt">
						<fo:table-row font-size="12pt">
							<fo:table-cell display-align="center" padding-left="1pt" padding-top="1pt" >
								<fo:block font-weight="bold">
									<fo:external-graphic content-height="11pt" height="11pt" src="siemens.jpg"/>
								</fo:block>
							</fo:table-cell>
							
							<fo:table-cell display-align="center" text-align="center" padding-left="2pt" padding-top="2pt"  number-columns-spanned="6">
								<fo:block font-weight="bold">
									<xsl:value-of select="$reportTitleHeading"/>
								</fo:block>
							</fo:table-cell>
							
							<fo:table-cell display-align="center" padding-left="1pt" padding-top="1pt">
								<fo:block font-weight="bold"></fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>
							<fo:table-cell display-align="center" padding-left="1pt" padding-top="1pt"  number-columns-spanned="8">
								<fo:block font-weight="bold">&#160;</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>		
							<fo:table-cell display-align="center" padding-left="1pt" padding-top="1pt"  number-columns-spanned="8">
								<fo:block font-weight="bold">&#160;</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="2pt" number-columns-spanned="8">
								<fo:block>
									<xsl:text>Einstellungen:</xsl:text>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="2pt" number-columns-spanned="8">
								<fo:block></fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="2pt" number-columns-spanned="8">
								<fo:block>
									<xsl:text>- Anzahl Ebenen: </xsl:text> <xsl:value-of select="$m0ProcessingLvl"/>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="2pt" number-columns-spanned="8">
								<fo:block>
									<xsl:text>- Inkl. LP-Struktur: </xsl:text> <xsl:value-of select="$PCBYesNo"/>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="2pt" number-columns-spanned="8">
								<fo:block>
									<xsl:text>- Konfigurationskontext: </xsl:text> <xsl:value-of select="$ConfigContext"/>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row font-size="6pt">						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Sachnummer</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Rev.</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Status</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Teileart</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Bezeichnung</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Äquiv. Teile Nr.</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Technische Info</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Freig. Datum</xsl:text>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
					
						<fo:table-row font-size="9pt">						
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt" padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='PartNumber']"/>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='Revision']"/>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='LifeCycleState']"/> 
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt" padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='PartType']"/>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black" height="0.93cm">
								<fo:block-container overflow="hidden" font-weight="bold" height="0.93cm">
									<fo:block font-weight="bold">
										<xsl:call-template name="zero_width_space_1">
											<xsl:with-param name="data" select="sbtrpt:Field[@Name='Nomenclature']"/>
										</xsl:call-template>
									</fo:block>
								</fo:block-container>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='L0DocCode']"/> 
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black" height="0.93cm">
								<fo:block-container overflow="hidden" font-weight="bold" height="0.93cm">
									<fo:block font-weight="bold">
										<xsl:call-template name="zero_width_space_1">
											<xsl:with-param name="data" select="sbtrpt:Field[@Name='l0TechInfo']"/>
										</xsl:call-template>   
									</fo:block>
								</fo:block-container>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='l0ReleaseDate']"/> 
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
					</fo:table-body>
					
				</xsl:when>
				
				<xsl:when test="$l0ReportLocale = 'EN'">
					<fo:table-body font-size="8pt">
						<fo:table-row font-size="12pt">
							<fo:table-cell display-align="center" padding-left="1pt" padding-top="1pt" >
								<fo:block font-weight="bold">
									<fo:external-graphic content-height="11pt" height="11pt" src="siemens.jpg"/>
								</fo:block>
							</fo:table-cell>
							
							<fo:table-cell display-align="center" text-align="center" padding-left="2pt" padding-top="2pt"  number-columns-spanned="6">
								<fo:block font-weight="bold">
									<xsl:value-of select="$reportTitleHeading"/>
								</fo:block>
							</fo:table-cell>
							
							<fo:table-cell display-align="center" padding-left="1pt" padding-top="1pt">
								<fo:block font-weight="bold"></fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>
							<fo:table-cell display-align="center" padding-left="1pt" padding-top="1pt"  number-columns-spanned="8">
								<fo:block font-weight="bold">&#160;</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>		
							<fo:table-cell display-align="center" padding-left="1pt" padding-top="1pt"  number-columns-spanned="8">
								<fo:block font-weight="bold">&#160;</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="2pt" number-columns-spanned="8">
								<fo:block>
									<xsl:text>Settings:</xsl:text>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="2pt" number-columns-spanned="8">
								<fo:block></fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="2pt" number-columns-spanned="8">
								<fo:block>
									<xsl:text>- Number of Level: </xsl:text> <xsl:value-of select="$m0ProcessingLvl"/>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="2pt" number-columns-spanned="8">
								<fo:block>
									<xsl:text>- Incl. PCBA: </xsl:text> <xsl:value-of select="$PCBYesNo"/>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row>						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="2pt" number-columns-spanned="8">
								<fo:block>
									<xsl:text>- Configuration Context: </xsl:text> <xsl:value-of select="$ConfigContext"/>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row font-size="6pt">						
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Identification no.</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Rev.</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Status</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Part type</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Designation</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Equiv. part no.</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Technical info</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="center" padding-left="2pt" padding-top="8pt">
								<fo:block>
									<xsl:text>Release date</xsl:text>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
						
						<fo:table-row font-size="9pt">						
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt" padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='PartNumber']"/>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='Revision']"/>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='LifeCycleState']"/> 
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt" padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='PartType']"/>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black" height="0.93cm">
								<fo:block-container overflow="hidden" font-weight="bold" height="0.93cm">
									<fo:block font-weight="bold">
										<xsl:call-template name="zero_width_space_1">
											<xsl:with-param name="data" select="sbtrpt:Field[@Name='Nomenclature']"/>
										</xsl:call-template>
									</fo:block>
								</fo:block-container>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='L0DocCode']"/> 
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black" height="0.93cm">
								<fo:block-container overflow="hidden" font-weight="bold" height="0.93cm">
									<fo:block font-weight="bold">
										<xsl:call-template name="zero_width_space_1">
											<xsl:with-param name="data" select="sbtrpt:Field[@Name='l0TechInfo']"/>
										</xsl:call-template>   
									</fo:block>
								</fo:block-container>
							</fo:table-cell>
							<fo:table-cell display-align="before" padding-left="2pt" padding-top="4pt"  padding-bottom="2pt" border-bottom="1pt solid black">
								<fo:block font-weight="bold">
									<xsl:value-of select="sbtrpt:Field[@Name='l0ReleaseDate']"/> 
								</fo:block>
							</fo:table-cell>
						</fo:table-row>
					</fo:table-body>
				</xsl:when>
			</xsl:choose>	
		</fo:table>
		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'DE'">
				
			<fo:table table-layout="fixed" width="27.7cm">
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="2.54cm"/>
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="2.5cm"/>
				<fo:table-column column-width="4.415cm"/>
				<fo:table-column column-width="4.415cm"/>
				<fo:table-column column-width="4.415cm"/>
				<fo:table-column column-width="4.415cm"/>
				
				
				<fo:table-body>

					<fo:table-row font-size="6pt">
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Ebene</xsl:text>
							</fo:block>
						</fo:table-cell>
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Pos.-Nr.</xsl:text>
							</fo:block>
						</fo:table-cell>
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Menge</xsl:text>
							</fo:block>
						</fo:table-cell>
						
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Einheit</xsl:text>
							</fo:block>
						</fo:table-cell>
						
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Sachnummer</xsl:text>
							</fo:block>
						</fo:table-cell>
						
						
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Rev.</xsl:text>
							</fo:block>
						</fo:table-cell>
						
						
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Status</xsl:text>
							</fo:block>
						</fo:table-cell>
						
						
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Teile-/Dokumentenart</xsl:text>
							</fo:block>
						</fo:table-cell>
						
						
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Bezeichnung de</xsl:text> 
							</fo:block>
						</fo:table-cell>
						
						
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Bezeichnung en</xsl:text>
							</fo:block>
						</fo:table-cell>
						
						
						<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
							<fo:block>
								<xsl:text>Technische Info</xsl:text>
							</fo:block>
						</fo:table-cell>
					</fo:table-row>
				</fo:table-body>
				
			</fo:table>
		</xsl:when>
			
		<xsl:when test="$l0ReportLocale = 'EN'">	
			<fo:table table-layout="fixed" width="27.7cm">
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="2.54cm"/>
				<fo:table-column column-width="1cm"/>
				<fo:table-column column-width="2.5cm"/>
				<fo:table-column column-width="4.415cm"/>
				<fo:table-column column-width="4.415cm"/>
				<fo:table-column column-width="4.415cm"/>
				<fo:table-column column-width="4.415cm"/>
				
					<fo:table-body>
						<fo:table-row font-size="6pt">
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Level</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Part ref.</xsl:text>
								</fo:block>
							</fo:table-cell>
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Quantity</xsl:text>
								</fo:block>
							</fo:table-cell>
							
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Unit</xsl:text>
								</fo:block>
							</fo:table-cell>
							
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Identification no.</xsl:text>
								</fo:block>
							</fo:table-cell>
							
							
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Rev.</xsl:text>
								</fo:block>
							</fo:table-cell>
							
							
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Status</xsl:text>
								</fo:block>
							</fo:table-cell>
							
							
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Part-/documenttype</xsl:text>
								</fo:block>
							</fo:table-cell>
							
							
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Designation de</xsl:text> 
								</fo:block>
							</fo:table-cell>
							
							
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Designation en</xsl:text>
								</fo:block>
							</fo:table-cell>
							
							
							<fo:table-cell display-align="before" text-align="left" padding-bottom="20pt" padding-left="2pt" padding-top="2pt">
								<fo:block>
									<xsl:text>Technical info</xsl:text>
								</fo:block>
							</fo:table-cell>
						</fo:table-row>	
					</fo:table-body>
				</fo:table>
			</xsl:when>
		</xsl:choose>
	
		<xsl:call-template name="doc_inc"/>
	</xsl:template>	
	

	
	<xsl:template name="body">
			<xsl:call-template name="recursion_controller" />
	</xsl:template>

	
	
	<xsl:template name="recursion_controller">
		<xsl:for-each select="sbtrpt:Relation[contains($part_rel_str, @Class)]/sbtrpt:Node">		
			<xsl:call-template name="rec_assm" />
		</xsl:for-each>
	</xsl:template>
	
	
	
	<xsl:template name="doc_inc">
		<xsl:for-each select="sbtrpt:Relation[contains($doc_rel_str, @Class)]/sbtrpt:Node">
			<xsl:call-template name="rec_doc" />		
		</xsl:for-each>
	</xsl:template>
	
	
	
	<xsl:template name="rec_doc">
		<xsl:param name="tmpline">		
			
			<xsl:value-of select="concat(
						'','&#254;', 
				../sbtrpt:Field[@Name='FindNumber'],'&#254;', 
				../sbtrpt:Field[@Name='DisplayQuantity'],'&#254;', 
				../sbtrpt:Field[@Name='UnitOfMeasure'],'&#254;',  
				sbtrpt:Field[@Name='DocumentName'],'&#254;', 
				sbtrpt:Field[@Name='Revision'],'&#254;', 
				sbtrpt:Field[@Name='LifeCycleState'],'&#254;', 
				sbtrpt:Field[@Name='DisplayedType'],'&#254;', 
				sbtrpt:Field[@Name='DocumentTitle'],'&#254;', 
				sbtrpt:Field[@Name='l0DocumentTitleEng'],'&#254;',
				'---','&#254;')"/>
		</xsl:param>

		<xsl:value-of select="js:add_result('0', $tmpline)"/>
			
	</xsl:template>
	
	
	
	<xsl:template name="rec_assm">
		<xsl:variable name="fnvalue">
			<xsl:call-template name="zero_width_space_1">
				<xsl:with-param name="data" select="../sbtrpt:Field[@Name='FindNumber']"/>
			</xsl:call-template>
		</xsl:variable>
		
		<xsl:variable name="refvalue">
			<xsl:call-template name="zero_width_space_1">
				<xsl:with-param name="data" select="../sbtrpt:Field[@Name='ReferenceDesignator']"/>
			</xsl:call-template>  
		</xsl:variable>
		
		<xsl:variable name="techvalue">
			<xsl:call-template name="zero_width_space_1">
				<xsl:with-param name="data" select="sbtrpt:Field[@Name='l0TechInfo']"/>
			</xsl:call-template>  
		</xsl:variable>
		
		<xsl:variable name="lvl">
			<xsl:value-of select="sbtrpt:Field[@Name='Level']" />
		</xsl:variable>
		
		<xsl:choose>
			<xsl:when test="$fnvalue = ''">
				<xsl:variable name="tmpline">
					<xsl:value-of select="concat(
						sbtrpt:Field[@Name='Level'],'&#254;', 
						$refvalue,'&#254;', 
						../sbtrpt:Field[@Name='DisplayQuantity'],'&#254;', 
						../sbtrpt:Field[@Name='UnitOfMeasure'],'&#254;',   
						sbtrpt:Field[@Name='PartNumber'],'&#254;',
						sbtrpt:Field[@Name='Revision'],'&#254;', 
						sbtrpt:Field[@Name='LifeCycleState'],'&#254;', 
						sbtrpt:Field[@Name='PartType'],'&#254;', 
						sbtrpt:Field[@Name='Nomenclature2'],'&#254;',
						sbtrpt:Field[@Name='Nomenclature'],'&#254;',
						$techvalue,'&#254;')"/>
				</xsl:variable>
			
				<xsl:value-of select="js:add_result($lvl, $tmpline)"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:variable name="tmpline">
					<xsl:value-of select="concat(
						sbtrpt:Field[@Name='Level'],'&#254;', 
						$fnvalue,'&#254;', 
						../sbtrpt:Field[@Name='DisplayQuantity'],'&#254;', 
						../sbtrpt:Field[@Name='UnitOfMeasure'],'&#254;',  
						sbtrpt:Field[@Name='PartNumber'],'&#254;',
						sbtrpt:Field[@Name='Revision'],'&#254;', 
						sbtrpt:Field[@Name='LifeCycleState'],'&#254;', 
						sbtrpt:Field[@Name='PartType'],'&#254;', 
						sbtrpt:Field[@Name='Nomenclature2'],'&#254;',						sbtrpt:Field[@Name='Nomenclature'],'&#254;',
						$techvalue,'&#254;')"/>
				</xsl:variable>
				
				<xsl:value-of select="js:add_result($lvl, $tmpline)"/>
			</xsl:otherwise>
		</xsl:choose>
		
		<xsl:call-template name="doc_inc" />
		
		<xsl:call-template name="rec_conf" />
	</xsl:template>
	
	
	<xsl:template name="rec_conf">
			<xsl:call-template name="recursion_controller" />
	</xsl:template>
	
	
	<xsl:template name="footer">
			<fo:table table-layout="fixed" width="27.7cm">
				<fo:table-column column-width="13.85cm"/>
				<fo:table-column column-width="13.85cm"/>
				<fo:table-body font-size="8pt">
					<fo:table-row>
						<fo:table-cell display-align="center" text-align="start" padding-left="2pt" padding-top="2pt"  border-top="1pt solid black">
							<fo:block>
								<xsl:choose>
									<xsl:when test="$l0ReportLocale = 'DE'">
										<xsl:text>&#xA9; Siemens Switzerland Ltd, Nur für internen Gebrauch</xsl:text>
									</xsl:when>
									<xsl:when test="$l0ReportLocale = 'EN'">
										<xsl:text>&#xA9; Siemens Switzerland Ltd, Restricted</xsl:text>
									</xsl:when>
								</xsl:choose>
							</fo:block>
						</fo:table-cell>
						<fo:table-cell display-align="center" text-align="end" number-columns-spanned="1" padding-right="2pt" padding-top="2pt" border-top="1pt solid black">
							<fo:block>
								<xsl:value-of select="$reportDate"/> 
								<xsl:text>, </xsl:text>
								<xsl:value-of select="$reportTime"/> 
								<xsl:choose>
									<xsl:when test="$l0ReportLocale = 'DE'">
										<xsl:text>, Seite </xsl:text>
									</xsl:when>
									<xsl:when test="$l0ReportLocale = 'EN'">
										<xsl:text>, Page </xsl:text>
									</xsl:when>
								</xsl:choose>
								<fo:page-number/>
								<xsl:text> / </xsl:text>
								<fo:page-number-citation ref-id="end"/>
							</fo:block>
						</fo:table-cell>
					</fo:table-row>
				</fo:table-body>
			</fo:table>
	</xsl:template>
	
</xsl:stylesheet>
