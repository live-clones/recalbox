<?xml version="1.0"?>
<xsl:stylesheet version="1.1" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

   <!-- sortie texte -->
   <xsl:output method="xml" indent="yes" encoding="UTF-8" doctype-public="-//Logiqx//DTD ROM Management Datafile//EN" doctype-system="http://www.logiqx.com/Dats/datafile.dtd"/>
   <xsl:strip-space elements="*"/>

   <xsl:template match="*">
      <xsl:copy>
         <xsl:apply-templates/>
      </xsl:copy>
   </xsl:template>

   <xsl:template match="/datafile/header/name">
      <name>Naomi</name>
   </xsl:template>
   <xsl:template match="/datafile/header/author">
      <author>Recalbox</author>
   </xsl:template>
   <xsl:template match="/datafile/header/homepage">
      <homepage>https://www.recalbox.com/</homepage>
   </xsl:template>
   <xsl:template match="/datafile/header/url"/>

   <xsl:template match="game|machine">
      <!--type du jeu-->
      <xsl:variable name="type">
         <xsl:choose>
            <xsl:when test="@isbios='yes'">
               <xsl:value-of select="'bios'"/>
            </xsl:when>
            <xsl:when test="@isdevice='yes'">
               <xsl:value-of select="'device'"/>
            </xsl:when>
            <xsl:when test="@runnable='no'">
               <xsl:value-of select="'not-runnable'"/>
            </xsl:when>
            <xsl:when test="@cloneof">
               <xsl:value-of select="'clone'"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="'parent'"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:variable>
      <!--bios du jeu-->
      <xsl:variable name="bios">
         <xsl:choose>
            <xsl:when test="not(@cloneof)"> <!--pas un clone-->
               <xsl:value-of select="@romof" />
            </xsl:when>
            <xsl:otherwise> <!--clone-->
               <xsl:variable name="pere" select="@romof" />
               <xsl:choose>
                  <xsl:when test="../game[@name=$pere]">
                     <xsl:value-of select="../game[@name=$pere]/@romof" />
                  </xsl:when>
                  <xsl:when test="../machine[@name=$pere]">
                     <xsl:value-of select="../machine[@name=$pere]/@romof" />
                  </xsl:when>
               </xsl:choose>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:variable>
      <!--filtre sur bios-->
      <xsl:if test="$bios='airlbios' or @name='airlbios' or $bios='naomi' or @name='naomi' or $bios='f355dlx' or @name='f355dlx' or $bios='f355bios' or @name='f355bios' or $bios='hod2bios' or @name='hod2bios'">
         <xsl:copy-of select="."/>
      </xsl:if>
   </xsl:template>
   

</xsl:stylesheet>
