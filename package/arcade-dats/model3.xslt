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
      <name>Model3</name>
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
      <!--pere du jeu-->
      <xsl:variable name="pere">
         <xsl:if test="$type='clone'">
            <xsl:value-of select="@cloneof" /> 
         </xsl:if>
         <xsl:if test="$type='parent'">
            <xsl:value-of select="@name" /> 
         </xsl:if>
      </xsl:variable>

      <!--filtre sur sourcefile-->
      <xsl:if test="@sourcefile='sega/model3.cpp'">
         <xsl:choose>
            <!-- eca et ses clones: ajout rom manquante -->
            <xsl:when test="$pere='eca'"> 
               <xsl:copy>
                  <xsl:copy-of select="@*"/>
                  <xsl:copy-of select="description"/>
                  <xsl:copy-of select="year"/>
                  <xsl:copy-of select="manufacturer"/>
                  <xsl:copy-of select="rom"/>
                  <xsl:if test="not(rom[@name='epr-19338a.bin'])">
                     <rom name="epr-19338a.bin" size="65536" crc="c9fac464" sha1="47b9ab7921a685c01629afb592d597faa11d2bd6" status="good"/>
                  </xsl:if>
                  <xsl:copy-of select="device_ref"/>
                  <xsl:copy-of select="driver"/>
               </xsl:copy>
            </xsl:when>               
            <!-- lemans24 et ses clones: ajout rom manquante -->
            <xsl:when test="$pere='lemans24'"> 
               <xsl:copy>
                  <xsl:copy-of select="@*"/>
                  <xsl:copy-of select="description"/>
                  <xsl:copy-of select="year"/>
                  <xsl:copy-of select="manufacturer"/>
                  <xsl:copy-of select="rom"/>
                  <xsl:if test="not(rom[@name='epr-18261.ic9'])">
                     <rom name="epr-18261.ic9" size="65536" crc="0c7fac58" sha1="68c1724c41401e28a5123022981c8919fd22656e"/>
                  </xsl:if>
                  <xsl:copy-of select="device_ref"/>
                  <xsl:copy-of select="driver"/>
               </xsl:copy>
            </xsl:when>
            <!-- fvipers2, vf3, vs2, vs298 et vs2v991 et leurs clones: ajout rom manquante -->
            <xsl:when test="$pere='fvipers2' or $pere='vf3' or $pere='vs2' or $pere='vs298' or $pere='vs2v991'">
               <xsl:copy>
                  <xsl:copy-of select="@*"/>
                  <xsl:copy-of select="description"/>
                  <xsl:copy-of select="year"/>
                  <xsl:copy-of select="manufacturer"/>
                  <xsl:copy-of select="rom"/>
                  <xsl:if test="not(rom[@name='epr-18022.ic2'])">
                     <rom name="epr-18022.ic2" size="65536" crc="0ca70f80" sha1="edf5ade72d9fa2f4d5f83f9f89e6cecfadd77f56"/>
                  </xsl:if>
                  <xsl:copy-of select="device_ref"/>
                  <xsl:copy-of select="driver"/>
               </xsl:copy>
            </xsl:when>
            <!-- sinon on recopie tel quel le jeu -->
            <xsl:otherwise>
               <xsl:copy-of select="."/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:if>


   </xsl:template>
   

</xsl:stylesheet>
