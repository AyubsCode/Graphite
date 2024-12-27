// for page navigation & to sort on leftbar

export type EachRoute = {
  title: string;
  href: string;
  noLink?: true;
  items?: EachRoute[];
};

export const ROUTES: EachRoute[] = [
  {
    title: "Getting Started",
    href: "/getting-started",
    noLink: true,
    items: [
      { title: "Introduction", href: "/introduction" },
      // {
      //   title: "Installation",
      //   href: "/installation",
      //   items: [
      //     { title: "Laravel", href: "/laravel" },
      //     { title: "React", href: "/react" },
      //     { title: "Gatsby", href: "/gatsby" },
      //     { title: "Test", href: "/test" },
      //   ],
      // },
    ],
  },
  {
    title: "Contributions",
    href: "/contributions",
    noLink: true,  // Parent item, no direct link
    items: [
      { title: "Documentation Contributions", href: "/documentation" },  // Clickable
      { 
        title: "Codebase Contributions",
        href: "/codebase" ,
        noLink: true , // dont set to false or else boom 
        items : [
          { title: "react-native", href: "/frontend" },  // Clickable
          // { title: "react-native", href: "/frontend" },  // Clickable
        ],
      },  // Clickable
    ]
  },
  {
    title : "React-Native Integration",
    href: "/react-native",
    noLink : true, 
    items : [
      {title: "react-native-reanimated" , href : "/react-native-reanimated"} , 
      {title: "Navigation" , href : "/routing"} , 
    ],
  },
  {
    title : "Timeline",
    href: "/progress",
  },
  // { We dont need this anymore . 
  //   title: "Design Document",  href: "/designdoc"  
  // },
  {
    title : "Database Integration",
    href: "/db",
    noLink : true, 
    items : [
      {title: "Relation Schematic" , href : "/schematic"} , 
      {title: "Supabase User Authentication" , href : "/supabase"} , 
      {title: "UUID Generation" , href : "/uuid"} , 
    ],
  },

];

type Page = { title: string; href: string };

function getRecurrsiveAllLinks(node: EachRoute) {
  const ans: Page[] = [];
  if (!node.noLink) {
    ans.push({ title: node.title, href: node.href });
  }
  node.items?.forEach((subNode) => {
    const temp = { ...subNode, href: `${node.href}${subNode.href}` };
    ans.push(...getRecurrsiveAllLinks(temp));
  });
  return ans;
}

export const page_routes = ROUTES.map((it) => getRecurrsiveAllLinks(it)).flat();
